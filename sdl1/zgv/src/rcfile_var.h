/* auto-generated from options.src, edits will be lost! */
struct cfglookup_tag cfglookup[]=
  {
  {"help",	0,	usage_help,	NULL},
  {"version",	0,	print_version,	NULL},
  {"auto-animate",	1,	get_bool,	&cfg.auto_animate},
  {"auto-mode-fit",	1,	get_bool,	&cfg.automodefit},
  {"avoid-single-progress",	1,	get_bool_and_inv,	&cfg.onefile_progress},
  {"black-background",	1,	get_bool,	&cfg.black_bg},
  {"block-cursor",	1,	get_bool,	&cfg.blockcursor},
  {"brightness",	1,	get_int,	&cfg.brightness},
  {"center",	1,	get_bool,	&cfg.centreflag},
  {"centre",	1,	get_bool,	&cfg.centreflag},
  {"clear-screen-on-exit",	1,	get_bool,	&cfg.cleartext},
  {"col-black",	1,	get_rgbval,	&cfg.black},
  {"col-dark",	1,	get_rgbval,	&cfg.dark},
  {"col-light",	1,	get_rgbval,	&cfg.light},
  {"col-medium",	1,	get_rgbval,	&cfg.medium},
  {"col-tagged",	1,	get_rgbval,	&cfg.marked},
  {"contrast",	1,	get_double,	&cfg.contrast},
  {"delete-single-prompt",	1,	get_bool_and_inv,	&cfg.nodelprompt},
  {"delete-tagged-prompt",	1,	get_bool_and_inv,	&cfg.nodelprompt_tagged},
  {"auto-mode-fit-diff",	1,	get_int,	&cfg.deltamodefit},
  {"dither-16col-fast",	1,	get_bool,	&cfg.fastdither16col},
  {"dither-hicol",	1,	get_bool,	&cfg.dither_hicol},
  {"fake-cols",	1,	get_bool,	&cfg.fakecols},
  {"force-fs-16col",	1,	get_bool,	&cfg.force16fs},
  {"force-viewer-8bit",	1,	get_bool_and_inv,	&cfg.jpeg24bit},
  {"fs-16col-color-thumbnails",	1,	get_bool,	&cfg.fs16col},
  {"fs-16col-colour-thumbnails",	1,	get_bool,	&cfg.fs16col},
  {"fs-ignore-old-pos",	1,	get_bool,	&cfg.forgetoldpos},
  {"fs-magic",	1,	get_bool,	&cfg.fsmagic},
  {"fs-perfect-cols",	1,	get_bool,	&cfg.perfectindex},
  {"fs-slow-thumbnail-update",	1,	get_bool,	&cfg.slowupdate},
  {"fs-small-text",	1,	get_bool,	&cfg.smallfstext},
  {"fs-start-mode",	1,	do_startmode,	&cfg.fs_startmode},
  {"fs-thick-text",	1,	get_bool,	&cfg.thicktext},
  {"fullscreen",	1,	get_bool,	&cfg.fullscreen},
  {"gamma",	1,	get_double,	&cfg.initial_picgamma},
  {"gnulitically-correct",	1,	get_bool,	&cfg.stupid_gnu_verbosity},
  {"ignore-errors",	1,	get_bool,	&cfg.errignore},
  {"jpeg-index-style",	1,	get_jis,	&cfg.jpegindexstyle},
  {"jpeg-speed",	1,	get_int,	&cfg.jpegspeed},
  {"line-text",	1,	get_bool,	&cfg.linetext},
  {"mouse",	1,	get_bool,	&cfg.svgalib_mouse},
  {"mouse-scale",	1,	get_int,	&cfg.mousescale},
  {"pcd-res",	1,	get_int,	&cfg.pcdres},
  {"pgm-truecol",	1,	get_bool,	&cfg.betterpgm},
  {"reverse-bc-order",	1,	get_bool,	&cfg.bc_order_rev},
  {"revert-orient",	1,	get_bool,	&cfg.revert_orient},
  {"revert-scale",	1,	get_bool,	&cfg.revert},
  {"scrollbar",	1,	get_bool,	&cfg.scrollbar},
  {"show-xvpics-dir",	1,	get_bool,	&cfg.showxvpicdir},
  {"slideshow-delay",	1,	get_int,	&cfg.tag_timer},
  {"slideshow-loop",	1,	get_bool,	&cfg.loop},
  {"slideshow-randomise",	1,	get_bool,	&cfg.shuffleslideshow},
  {"slideshow-randomize",	1,	get_bool,	&cfg.shuffleslideshow},
  {"viewer-16col-color",	1,	get_bool,	&cfg.viewer16col},
  {"viewer-16col-colour",	1,	get_bool,	&cfg.viewer16col},
  {"viewer-start-mode",	1,	do_startmode,	&cfg.videomode},
  {"vkludge",	1,	get_bool,	&cfg.vkludge},
  {"visual",	1,	get_bool,	&cfg.xvpic_index},
  {"xzgv-keys",	1,	get_bool,	&cfg.xzgvkeys},
  {"zoom",	1,	get_bool,	&cfg.zoom},
  {"zoom-reduce-only",	1,	get_bool,	&cfg.zoom_reduce_only},
  {"reload-delay",	0,	get_int,	&cfg.repeat_timer},
  {"show-tagged",	0,	get_bool,	&cfg.echotagged},
  {"show-dimensions",	0,	get_bool,	&cfg.selecting},
  {"write-ppm",	0,	get_bool,	&cfg.writefile},
  {"mode-all-bad",	1,	do_allmodesbad,	NULL},
  {"mode-all-good",	1,	do_allmodesgood,	NULL},
  {"mode-bad",	1,	do_badmode,	NULL},
  {"mode-good",	1,	do_goodmode,	NULL},
  {"mouse-kludge",	1,	get_bool,	&cfg.mousekludge},
  {"allmodesbad",	1,	do_allmodesbad,	NULL},
  {"allmodesgood",	1,	do_allmodesgood,	NULL},
  {"automodefit",	1,	get_bool,	&cfg.automodefit},
  {"badmode",	1,	do_badmode,	NULL},
  {"bc_order_rev",	1,	get_bool,	&cfg.bc_order_rev},
  {"betterpgm",	1,	get_bool,	&cfg.betterpgm},
  {"black",	1,	get_rgbval,	&cfg.black},
  {"blockcursor",	1,	get_bool,	&cfg.blockcursor},
  {"cleartext",	1,	get_bool,	&cfg.cleartext},
  {"dark",	1,	get_rgbval,	&cfg.dark},
  {"fakecols",	1,	get_bool,	&cfg.fakecols},
  {"fastdither16col",	1,	get_bool,	&cfg.fastdither16col},
  {"force16fs",	1,	get_bool,	&cfg.force16fs},
  {"forgetoldpos",	1,	get_bool,	&cfg.forgetoldpos},
  {"fs16col",	1,	get_bool,	&cfg.fs16col},
  {"fs_startmode",	1,	do_startmode,	&cfg.fs_startmode},
  {"fsmagic",	1,	get_bool,	&cfg.fsmagic},
  {"gnulitically_correct",	1,	get_bool,	&cfg.stupid_gnu_verbosity},
  {"goodmode",	1,	do_goodmode,	NULL},
  {"jpeg24bit",	1,	get_bool,	&cfg.jpeg24bit},
  {"jpegindexstyle",	1,	get_jis,	&cfg.jpegindexstyle},
  {"jpegspeed",	1,	get_int,	&cfg.jpegspeed},
  {"light",	1,	get_rgbval,	&cfg.light},
  {"linetext",	1,	get_bool,	&cfg.linetext},
  {"medium",	1,	get_rgbval,	&cfg.medium},
  {"mousekludge",	1,	get_bool,	&cfg.mousekludge},
  {"mousescale",	1,	get_int,	&cfg.mousescale},
  {"nodelprompt",	1,	get_bool,	&cfg.nodelprompt},
  {"onefile_progress",	1,	get_bool,	&cfg.onefile_progress},
  {"pcdres",	1,	get_int,	&cfg.pcdres},
  {"perfectindex",	1,	get_bool,	&cfg.perfectindex},
  {"revert",	1,	get_bool,	&cfg.revert},
  {"revert_orient",	1,	get_bool,	&cfg.revert_orient},
  {"showxvpicdir",	1,	get_bool,	&cfg.showxvpicdir},
  {"shuffleslideshow",	1,	get_bool,	&cfg.shuffleslideshow},
  {"slowupdate",	1,	get_bool,	&cfg.slowupdate},
  {"smallfstext",	1,	get_bool,	&cfg.smallfstext},
  {"startmode",	1,	do_startmode,	&cfg.videomode},
  {"tagged",	1,	get_rgbval,	&cfg.marked},
  {"tagtimeout",	1,	get_int,	&cfg.tag_timer},
  {"thicktext",	1,	get_bool,	&cfg.thicktext},
  {"viewer16col",	1,	get_bool,	&cfg.viewer16col},
  {"xzgvkeys",	1,	get_bool,	&cfg.xzgvkeys},
  {"zoom_reduce_only",	1,	get_bool,	&cfg.zoom_reduce_only},
  {"fullsel",	1,	do_warn,	"fullsel"},
  {"hicolmodes",	1,	do_warn,	"hicolmodes"},
  {"hicontrol",	1,	do_warn,	"hicontrol"},
  {"",0,NULL,NULL}
  };
