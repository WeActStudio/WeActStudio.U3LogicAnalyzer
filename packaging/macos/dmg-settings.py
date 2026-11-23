# This file is part of the LogicAnalyzer project.
#
# Copyright (C) 2026 WeAct Studio
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# dmgbuild settings for the U3LogicAnalyzer disk image.
# Called by package_installer.sh; dmgbuild writes the .DS_Store itself, which
# avoids Finder automation - that is refused in a non-interactive session.
app_name = 'U3LogicAnalyzer.app'
app_path = defines['app_path']
bg = defines['bg_path']

files = [app_path]
symlinks = {'Applications': '/Applications'}
# Must match ICON_Y and the arrow span painted into dmg-background.png.
icon_locations = {app_name: (150, 165), 'Applications': (450, 165)}
background = bg
# WindowBounds is the whole window frame, title bar included, so the heights
# add up to 400 for the background to fit without being cropped at the bottom.
window_rect = ((240, 200), (600, 428))
default_view = 'icon-view'
icon_size = 128
text_size = 13
show_status_bar = False
show_tab_view = False
show_toolbar = False
show_pathbar = False
show_sidebar = False
arrange_by = None
label_pos = 'bottom'
