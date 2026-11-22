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
