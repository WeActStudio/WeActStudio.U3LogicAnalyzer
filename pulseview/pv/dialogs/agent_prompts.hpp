/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2026 WeAct Studio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PULSEVIEW_PV_DIALOGS_AGENTPROMPTS_HPP
#define PULSEVIEW_PV_DIALOGS_AGENTPROMPTS_HPP

#include <QDialog>

namespace pv {
namespace dialogs {

/** One prompt as the dialog shows it: a short title and the text to paste. */
struct AgentPromptEntry {
	const char *title;
	const char *body;
};

/**
 * Ready-made prompts for an AI assistant, one card each, each with a copy
 * button: the user pastes the text and does not have to retype it.
 */
class AgentPrompts : public QDialog
{
	Q_OBJECT

public:
	explicit AgentPrompts(QWidget *parent = nullptr);

private:
	QWidget *create_card(const AgentPromptEntry &entry, QWidget *parent);

	/*
	 * The five bodies only exist in the two languages the builds ship
	 * complete text for, so they are kept out of tr(): going through it
	 * would put every one of them into all four .ts files, untranslated.
	 * Titles and button labels are short and do use tr().
	 */
	static const AgentPromptEntry EntriesZh[];
	static const AgentPromptEntry EntriesEn[];
	static const int EntryCount = 5;
};

} // namespace dialogs
} // namespace pv

#endif // PULSEVIEW_PV_DIALOGS_AGENTPROMPTS_HPP
