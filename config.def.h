/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx        = 2;        /* border pixel of windows */
static unsigned int gappx           = 6;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int swterminheritfs    = 1;        /* 1 terminal inherits fullscreen on unswallow, 0 otherwise */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 32;        /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static char font[]                  = "monospace:size=10";
static char dmenufont[]             = "monospace:size=10";
static const char *fonts[]          = { font };
static char statusbarcolor[]        = "#0a0a0a";
static char col_gray2[]             = "#444444";
static char col_gray3[]             = "#9A8F8A";
static char col_cyan[]              = "#6E7F96";
static char *colors[][3]            = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray2, statusbarcolor, col_gray2 },
	[SchemeSel]  = { col_gray3, statusbarcolor, col_cyan  },
};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "font",               STRING,  &font },
	{ "statusbarcolor",     STRING,  &statusbarcolor },
	{ "borderpx",           INTEGER, &borderpx },
};

static char *tagsel[][2] = {
   /*   fg         bg    */
  { col_gray2, statusbarcolor },  /* norm */
  { col_cyan,  statusbarcolor }, /* sel */
  { col_gray2, statusbarcolor },  /* occ but not sel */
  { col_cyan,  statusbarcolor },  /* has pinned tag */
};
/* tagging */
/* tagging: refer to https://github.com/bakkeby/patches/wiki/tagicons */
static char *tagicons[][NUMTAGS] = {
	[IconsDefault]        = { "" },
	[IconsVacant]         = { "1", "2", "3", "4", "5", "6", "7", "8", "9" },
	[IconsOccupied]       = { "1", "2", "3", "4", "5", "6", "7", "8", "9" },
};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance    title            tags mask  isfloating  isterminal  noswallow  monitor unmanaged*/
	{ "Gimp",         NULL,       NULL,            0,         1,          0,           0,        -1,         0 },
	{ "Firefox",      NULL,       NULL,            1 << 8,    0,          0,          -1,        -1,         0 },
	{ "st-256color",  NULL,       NULL,            0,         0,          1,           0,        -1,         0 },
	{ NULL,           NULL,       "Event Tester",  0,         0,          0,           1,        -1,         0 }, /* xev */
	{ "Eww",          NULL,       NULL,            0,         0,          0,           0,        -1,         1 },
};

/* layout(s) */
static const float mfact     = 0.66; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */


static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "󰙀",      tilewide },
	{ "󰄶",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
	{ "󰙀",      tile },    /* first entry is default */
	{ "",      centeredmaster },
	{ "󰅨",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", statusbarcolor, "-nf", col_gray3, "-sb", col_cyan, "-sf", statusbarcolor, NULL };
static const char *termcmd[]  = { "usr-terminal", NULL };
static const char *applaunchercmd[]  = { "usr-applauncher", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_space,  spawn,          {.v = applaunchercmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_w,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_f,      focusmaster,    {0} },
	{ MODKEY|ShiftMask,             XK_j,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ControlMask,           XK_m,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_r,      livereloadxrdb, {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_r,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        moveorplace,    {.i = 1} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizeormfact,  {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTagBar,            0,              Button4,        cycleiconset,   {.i = +1 } },
	{ ClkTagBar,            0,              Button5,        cycleiconset,   {.i = -1 } },
};

