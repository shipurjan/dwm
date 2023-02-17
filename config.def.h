/* See LICENSE file for copyright and license details. */

#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const Gap default_gap        = {.isgap = 0, .realgap = 50, .gappx = 0};
static const unsigned int snap      = 8;       /* snap pixel */
static const unsigned int minwsz    = 20;       /* Minimal heigt of a client for smfact */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=12", "NotoColorEmoji:pixelsize=12:antialias=true:autohint=true"  };
static const char dmenufont[]       = "monospace:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_darkpurple[]  = "#4b084c";
static const char col_lightpurple[] = "#c615c8";
static const char col_lightyellow[] = "#f9f9c8";
static const char col_pumpkin[]     = "#ff7518";
static const char col_cyan[]        = "#005577";
static const char col_green[]       = "#00ff00";
static const char col_red[]         = "#ff0000";
static const char col_black[]       = "#000000";
static const char col_white[]       = "#ffffff";
static const char *colors[][3]      = {
	/*               fg               bg              border   */
	[SchemeNorm] = { col_white,       col_black,      col_black },
	[SchemeSel]  = { col_lightyellow, col_darkpurple, col_lightpurple },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance    title       tags mask     isfloating   monitor */
	{ "Gimp",         NULL,       NULL,       0,            1,           -1 },
	{ "mpv",          NULL,       NULL,       0,            1,           -1 },
  { "qBittorrent",  NULL,       NULL,       1 << 6,       0,            1 },
  { "discord",      NULL,       NULL,       1 << 5,       0,            1 },
  { "Steam",        NULL,       NULL,       1 << 4,       0,            1 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const float smfact    = 0.00; /* factor of tiled clients [0.00..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
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

#define STATUSBAR "dwmblocks"

/* commands */
static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-nb", col_black, "-nf", col_white, "-sb", col_darkpurple, "-sf", col_lightyellow, NULL };
static const char *termcmd[]  = { "alacritty", NULL };


static const Key keys[] = {
	/* modifier                     key               function        argument */
	{ MODKEY,                       XK_d,             spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,        spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_KP_Enter,      spawn,          {.v = termcmd } },
  { MODKEY|ShiftMask,             XK_Return,        focusmaster,    {0} },
	{ MODKEY,                       XK_b,             togglebar,      {0} },
	{ MODKEY,                       XK_j,             focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,             focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,             movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,             movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask|ControlMask, XK_j,             rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask|ControlMask, XK_k,             rotatestack,    {.i = -1 } },
	{ MODKEY,                       XK_i,             incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_u,             incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,             setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,             setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,             setsmfact,      {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_l,             setsmfact,      {.f = -0.05} },
	{ MODKEY,                       XK_Return,        zoom,           {0} },
	{ MODKEY,                       XK_Tab,           view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,             killclient,     {0} },
	{ MODKEY,                       XK_t,             setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,             setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,             setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,         setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,         togglefloating, {0} },
	{ MODKEY,                       XK_0,             view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,             tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,         focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,        focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,         tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,        tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,         setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_KP_Subtract,   setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_equal,         setgaps,        {.i = +5 } },
	{ MODKEY,                       XK_KP_Add,        setgaps,        {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_minus,         setgaps,        {.i = GAP_TOGGLE} },
	{ MODKEY|ShiftMask,             XK_KP_Subtract,   setgaps,        {.i = GAP_TOGGLE} },
	{ MODKEY|ShiftMask,             XK_equal,         setgaps,        {.i = GAP_RESET } },
	{ MODKEY|ShiftMask,             XK_KP_Add,        setgaps,        {.i = GAP_RESET } },
	TAGKEYS(                        XK_1,                             0)
	TAGKEYS(                        XK_KP_End,                        0)
	TAGKEYS(                        XK_2,                             1)
	TAGKEYS(                        XK_KP_Down,                       1)
	TAGKEYS(                        XK_3,                             2)
	TAGKEYS(                        XK_KP_Page_Down,                  2)
	TAGKEYS(                        XK_4,                             3)
	TAGKEYS(                        XK_KP_Left,                       3)
	TAGKEYS(                        XK_5,                             4)
	TAGKEYS(                        XK_KP_Begin,                      4)
	TAGKEYS(                        XK_6,                             5)
	TAGKEYS(                        XK_KP_Right,                      5)
	TAGKEYS(                        XK_7,                             6)
	TAGKEYS(                        XK_KP_Home,                       6)
	{ MODKEY|ShiftMask,             XK_p,             quit,           {1} },
	{ MODKEY|ControlMask|ShiftMask, XK_p,             quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigstatusbar,   {.i = 6} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
