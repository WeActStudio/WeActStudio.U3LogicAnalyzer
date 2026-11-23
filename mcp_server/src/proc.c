/*
 * This file is part of the LogicAnalyzer project.
 *
 * Copyright (C) 2026 WeAct Studio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "proc.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#else
# include <fcntl.h>
# include <poll.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#endif

void proc_result_init(proc_result *result)
{
	result->started = 0;
	result->exit_code = -1;
	result->timed_out = 0;
	sb_init(&result->out);
	sb_init(&result->err);
}

void proc_result_free(proc_result *result)
{
	sb_free(&result->out);
	sb_free(&result->err);
}

#ifdef _WIN32

# include <windows.h>

/* Wide string from UTF-8, freed by the caller. */
static wchar_t *to_wide(const char *text)
{
	int need = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
	wchar_t *out;

	if (need <= 0)
		return NULL;
	out = malloc((size_t)need * sizeof(wchar_t));
	if (!out)
		return NULL;
	if (!MultiByteToWideChar(CP_UTF8, 0, text, -1, out, need)) {
		free(out);
		return NULL;
	}
	return out;
}

/*
 * Command line quoting as CommandLineToArgvW undoes it: wrap in quotes when
 * the argument has a space, and double the backslashes in front of a quote.
 */
static void append_quoted(strbuf *cmdline, const char *arg)
{
	int need_quotes = *arg == '\0' || strpbrk(arg, " \t\"") != NULL;
	size_t backslashes = 0;

	if (!need_quotes) {
		sb_put(cmdline, arg);
		return;
	}
	sb_putc(cmdline, '"');
	for (const char *p = arg; ; p++) {
		if (*p == '\\') {
			backslashes++;
			continue;
		}
		if (*p == '"') {
			for (size_t i = 0; i < backslashes * 2 + 1; i++)
				sb_putc(cmdline, '\\');
			sb_putc(cmdline, '"');
			backslashes = 0;
			continue;
		}
		for (size_t i = 0; i < backslashes; i++)
			sb_putc(cmdline, '\\');
		backslashes = 0;
		if (!*p)
			break;
		sb_putc(cmdline, *p);
	}
	/* Trailing backslashes would escape the closing quote. */
	for (size_t i = 0; i < backslashes; i++)
		sb_putc(cmdline, '\\');
	sb_putc(cmdline, '"');
}

static int drain(HANDLE pipe, strbuf *into)
{
	char chunk[4096];
	DWORD available = 0, got = 0;

	if (!PeekNamedPipe(pipe, NULL, 0, NULL, &available, NULL))
		return 0;
	if (!available)
		return 1;
	if (!ReadFile(pipe, chunk, (DWORD)sizeof(chunk), &got, NULL) || got == 0)
		return 0;
	sb_putn(into, chunk, got);
	return 1;
}

int proc_run(char *const argv[], const char *workdir, int timeout_sec,
	     proc_result *result, char *err, size_t errlen)
{
	strbuf cmdline;
	SECURITY_ATTRIBUTES inherit;
	HANDLE out_read = NULL, out_write = NULL, err_read = NULL, err_write = NULL;
	STARTUPINFOW startup;
	PROCESS_INFORMATION process;
	wchar_t *wide_cmdline = NULL, *wide_dir = NULL;
	DWORD waited;
	int ok = -1;

	if (!argv || !argv[0]) {
		snprintf(err, errlen, "no program to run");
		return -1;
	}

	sb_init(&cmdline);
	for (size_t i = 0; argv[i]; i++) {
		if (i)
			sb_putc(&cmdline, ' ');
		append_quoted(&cmdline, argv[i]);
	}

	inherit.nLength = sizeof(inherit);
	inherit.bInheritHandle = TRUE;
	inherit.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&out_read, &out_write, &inherit, 0) ||
	    !CreatePipe(&err_read, &err_write, &inherit, 0)) {
		snprintf(err, errlen, "CreatePipe failed (%lu)", (unsigned long)GetLastError());
		goto done;
	}
	SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0);

	memset(&startup, 0, sizeof(startup));
	startup.cb = sizeof(startup);
	startup.dwFlags = STARTF_USESTDHANDLES;
	startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startup.hStdOutput = out_write;
	startup.hStdError = err_write;
	memset(&process, 0, sizeof(process));

	wide_cmdline = to_wide(cmdline.data);
	if (workdir)
		wide_dir = to_wide(workdir);
	/* CREATE_NO_WINDOW: an agent that calls a tool must not make a console
	 * window flash up on the desktop. */
	if (!wide_cmdline || !CreateProcessW(NULL, wide_cmdline, NULL, NULL, TRUE,
					     CREATE_NO_WINDOW, NULL, wide_dir,
					     &startup, &process)) {
		snprintf(err, errlen, "could not start %s (%lu)", argv[0],
			 (unsigned long)GetLastError());
		goto done;
	}

	CloseHandle(out_write);
	out_write = NULL;
	CloseHandle(err_write);
	err_write = NULL;

	{
		DWORD deadline = GetTickCount() + (DWORD)(timeout_sec > 0 ? timeout_sec : 30) * 1000;
		int out_open = 1, err_open = 1;

		while (out_open || err_open) {
			if (!drain(out_read, &result->out))
				out_open = 0;
			if (!drain(err_read, &result->err))
				err_open = 0;
			if (!out_open && !err_open)
				break;
			if (GetTickCount() > deadline) {
				TerminateProcess(process.hProcess, 1);
				result->timed_out = 1;
				/* Whatever is still buffered is worth keeping. */
				while (drain(out_read, &result->out)) { }
				while (drain(err_read, &result->err)) { }
				break;
			}
			Sleep(20);
		}
	}

	waited = WaitForSingleObject(process.hProcess, 5000);
	if (waited == WAIT_OBJECT_0) {
		DWORD code = 0;

		if (GetExitCodeProcess(process.hProcess, &code))
			result->exit_code = (int)code;
	}
	CloseHandle(process.hThread);
	CloseHandle(process.hProcess);
	result->started = 1;
	ok = 0;

done:
	if (out_write)
		CloseHandle(out_write);
	if (err_write)
		CloseHandle(err_write);
	if (out_read)
		CloseHandle(out_read);
	if (err_read)
		CloseHandle(err_read);
	free(wide_cmdline);
	free(wide_dir);
	sb_free(&cmdline);
	return ok;
}

#else

static int read_available(int fd, strbuf *into)
{
	char chunk[4096];
	ssize_t got = read(fd, chunk, sizeof(chunk));

	if (got <= 0)
		return 0;
	sb_putn(into, chunk, (size_t)got);
	return 1;
}

int proc_run(char *const argv[], const char *workdir, int timeout_sec,
	     proc_result *result, char *err, size_t errlen)
{
	int out_pipe[2], err_pipe[2];
	pid_t pid;
	int status = 0;
	int out_open = 1, err_open = 1;

	if (!argv || !argv[0]) {
		snprintf(err, errlen, "no program to run");
		return -1;
	}
	if (pipe(out_pipe) != 0 || pipe(err_pipe) != 0) {
		snprintf(err, errlen, "pipe failed: %s", strerror(errno));
		return -1;
	}

	pid = fork();
	if (pid < 0) {
		snprintf(err, errlen, "fork failed: %s", strerror(errno));
		close(out_pipe[0]);
		close(out_pipe[1]);
		close(err_pipe[0]);
		close(err_pipe[1]);
		return -1;
	}
	if (pid == 0) {
		dup2(out_pipe[1], STDOUT_FILENO);
		dup2(err_pipe[1], STDERR_FILENO);
		close(out_pipe[0]);
		close(out_pipe[1]);
		close(err_pipe[0]);
		close(err_pipe[1]);
		if (workdir && chdir(workdir) != 0)
			_exit(127);
		execvp(argv[0], argv);
		_exit(127);
	}

	close(out_pipe[1]);
	close(err_pipe[1]);
	fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);
	fcntl(err_pipe[0], F_SETFL, O_NONBLOCK);

	{
		long long deadline_ms = (long long)(timeout_sec > 0 ? timeout_sec : 30) * 1000;
		long long elapsed = 0;

		while (out_open || err_open) {
			struct pollfd fds[2];
			int count = 0, ready;
			int out_idx = -1, err_idx = -1;

			if (out_open) {
				fds[count].fd = out_pipe[0];
				fds[count].events = POLLIN;
				out_idx = count++;
			}
			if (err_open) {
				fds[count].fd = err_pipe[0];
				fds[count].events = POLLIN;
				err_idx = count++;
			}
			ready = poll(fds, (nfds_t)count, 50);
			if (ready > 0) {
				if (out_idx >= 0 && (fds[out_idx].revents & (POLLIN | POLLHUP))) {
					if (!read_available(out_pipe[0], &result->out))
						out_open = 0;
				}
				if (err_idx >= 0 && (fds[err_idx].revents & (POLLIN | POLLHUP))) {
					if (!read_available(err_pipe[0], &result->err))
						err_open = 0;
				}
			}
			if (!out_open && !err_open)
				break;
			elapsed += 50;
			if (elapsed > deadline_ms) {
				kill(pid, SIGKILL);
				result->timed_out = 1;
				out_open = err_open = 0;
				break;
			}
		}
	}

	if (waitpid(pid, &status, 0) == pid) {
		if (WIFEXITED(status))
			result->exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			result->exit_code = 128 + WTERMSIG(status);
	}
	close(out_pipe[0]);
	close(err_pipe[0]);
	result->started = 1;
	return 0;
}

#endif
