#include "lvgl/lvgl.h"

#define TAB_LEFT_APP_COUNT      (4)     // APP的直接个数
#define TAB_MAIN_APP_COUNT      (11)    // TAB_LEFT_APP_COUNT至TAB_MAIN_APP_COUNT之间APP的放在这里
#define TAB_RIGHT_APP_COUNT     (11)    // 剩下的APP都放到这里(大于TAB_MAIN_APP_COUNT)
#define MENU_TABLE_TIPS_PROMPT  (10)     // 菜单位置提示大小

#define ICON_SIZE           (64)
#define ICON_ROW_COUNT      (4)
#define ICON_COLUNM_COUNT   (6)
#define ICON_PAD_TOP        (40)
#define ICON_PAD_BOTTOM     (40)
#define ICON_PAD_LEFT       (115)
#define ICON_PAD_RIGHT      (115)

#define ICON_ROW_SPACE      (60)
#define ICON_COL_SPACE      (90)//((ICON_HOR_RES - (ICON_SIZE * ICON_COLUNM_COUNT)) / (ICON_COLUNM_COUNT - 1))
#define ICON_HOR_RES        (4 + (ICON_SIZE * ICON_COLUNM_COUNT) + (ICON_COL_SPACE * (ICON_COLUNM_COUNT - 1)))//((LV_HOR_RES - ICON_PAD_LEFT - ICON_PAD_RIGHT))        // 列间距
#define ICON_VER_RES        (4 + (ICON_SIZE * ICON_ROW_COUNT) + (ICON_ROW_SPACE * (ICON_ROW_COUNT - 1)))//((LV_VER_RES - ICON_PAD_TOP  - ICON_PAD_BOTTOM))       // 行间距

#define LV_100ASK_MENU_ITEM_COUNT   (14)

static uint16_t g_tab_act = 1;  // 更新tabview的索引，用于返回桌面时保持原来的位置


LV_IMG_DECLARE(img_100ask_logo);
LV_IMG_DECLARE(img_lv_100ask_icon_about);
LV_IMG_DECLARE(img_lv_100ask_icon_benchmark);
LV_IMG_DECLARE(img_lv_100ask_icon_calc);
LV_IMG_DECLARE(img_lv_100ask_icon_fileManager);
LV_IMG_DECLARE(img_lv_100ask_icon_game2048);
LV_IMG_DECLARE(img_lv_100ask_icon_gameMemory);
LV_IMG_DECLARE(img_lv_100ask_icon_LAN);
LV_IMG_DECLARE(img_lv_100ask_icon_music);
LV_IMG_DECLARE(img_lv_100ask_icon_setBrightness);
LV_IMG_DECLARE(img_lv_100ask_icon_setTime);
LV_IMG_DECLARE(img_lv_100ask_icon_snake);
LV_IMG_DECLARE(img_lv_100ask_icon_tiles);
LV_IMG_DECLARE(img_lv_100ask_icon_widgets);
LV_IMG_DECLARE(img_lv_100ask_icon_WLAN);
///LV_IMG_DECLARE(img_lv_100ask_icon_bg);

/* 桌标图标数组 */
static const lv_img_dsc_t *app_icon[] = \
    { \
      &img_lv_100ask_icon_about,            &img_lv_100ask_icon_benchmark,  &img_lv_100ask_icon_calc, \
      &img_lv_100ask_icon_fileManager,      &img_lv_100ask_icon_game2048,   &img_lv_100ask_icon_gameMemory, \
      &img_lv_100ask_icon_LAN,              &img_lv_100ask_icon_music,      &img_lv_100ask_icon_setBrightness, \
      &img_lv_100ask_icon_setTime,          &img_lv_100ask_icon_snake,      &img_lv_100ask_icon_tiles, \
      &img_lv_100ask_icon_widgets,          &img_lv_100ask_icon_WLAN};

/* 应用图标名称 */
static const char * app_name[] = \
    { \
      "About",          "Benchmar", "Calc", \
      "File Manager",   "2048",     "Memor", \
      "LAN",            "Music",    "Brightness", \
      "Time",           "Snake",    "Tiles", \
      "Widgets",        "WLAN"
    };

static void init_icon(void);
static void set_menu_table_tips(lv_obj_t * parent, int count);
static void lcd_top_widgets(lv_obj_t * parent);

static void get_tab_act_index_event_handler(lv_event_t * e);

static void clean_screen_obj(lv_obj_t * parent);



void lv_100ask_desktop(void)
{
    init_icon();
}



static void init_icon(void)
{
    int app_count = 1;              // 计数以区分不同的tab页面

    lv_obj_t * img_gb;              // 桌面背景
    lv_obj_t * img_icon;            // APP图标
    lv_obj_t * label_icon;          // 基于APP图标创建的名称，点击时图标时提取，不展示出来
    lv_obj_t * label_icon_name;     // 展示在图标下方的名称
    lv_obj_t * tabview_desktop;     // tab总页面
    lv_obj_t * img_bottom_icon;     // 展示在底部快速访问栏的图标
    lv_obj_t * label_bottom_icon;   // 基于底部快速访问栏的图标创建的名称，点击时图标时提取，不展示出来

    lv_obj_t * tab_left;            // 左边的tab页面
    lv_obj_t * tab_main;            // 中间的tab页面
    lv_obj_t * tab_right;           // 右边的tab页面
    lv_obj_t * icon_cont_left;      // 中间图标区域面板
    lv_obj_t * icon_cont_main;      // 中间图标区域面板
    lv_obj_t * icon_cont_right;     // 中间图标区域面板

    static lv_style_t style_tabview_desktop;    // 容器的样式
  	static lv_style_t cont_style;               // 中间图标区域，容器的样式
	static lv_style_t icon_style;               // 中间图标区域，容器中的图标的样式
    static lv_style_t obj_bottom_panel_style;   // 底部容器的样式

    if (lv_obj_get_child(lv_scr_act(), 0))
        lv_obj_del(lv_obj_get_child(lv_scr_act(), 0));
    
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    //lv_style_reset(&cont_style);
    //lv_style_reset(&icon_style);
    //lv_style_reset(&obj_bottom_panel_style);

    lv_style_init(&style_tabview_desktop);
    //lv_style_set_pad_all(&style_tabview_desktop, 0);
    lv_style_set_bg_opa(&style_tabview_desktop, 0);
    //lv_style_set_shadow_opa(&style_tabview_desktop, 0);
    //lv_style_set_border_opa(&style_tabview_desktop, 0);

	/* 设置容器的样式 */
	lv_style_init(&cont_style);
    lv_style_set_bg_opa(&cont_style, 0);
    lv_style_set_border_opa(&cont_style, 0);
    lv_style_set_pad_column(&cont_style, ICON_COL_SPACE);
    lv_style_set_pad_row(&cont_style, ICON_ROW_SPACE);
    lv_style_set_pad_all(&cont_style, 0);
    lv_style_set_layout(&cont_style, LV_LAYOUT_FLEX);
    lv_style_set_base_dir(&cont_style, LV_BASE_DIR_LTR);
    lv_style_set_flex_flow(&cont_style, LV_FLEX_FLOW_ROW_WRAP);

	/* 容器中的图标的样式 */
	lv_style_init(&icon_style);
    lv_style_set_text_opa(&icon_style, 0);
    lv_style_set_text_font(&icon_style,  &lv_font_montserrat_8);
	//lv_style_set_radius(&icon_style, 0);
    //lv_style_set_border_width(&icon_style, 1);

    /* 底部面板区域 */
    lv_style_init(&obj_bottom_panel_style);
    lv_style_set_pad_all(&obj_bottom_panel_style, 0);
    lv_style_set_bg_opa(&obj_bottom_panel_style, LV_OPA_50);
    lv_style_set_pad_left(&obj_bottom_panel_style, 10);
    lv_style_set_pad_right(&obj_bottom_panel_style, 10);
    //lv_style_set_shadow_opa(&obj_bottom_panel_style, 0);
    lv_style_set_border_opa(&obj_bottom_panel_style, 0);
    lv_style_set_radius(&obj_bottom_panel_style, 22);

    // 桌面背景 TODO
#if 0
    ///img_gb = lv_img_create(lv_scr_act());
    ///lv_img_set_src(img_gb, &img_lv_100ask_icon_bg);
#else
    img_gb = lv_obj_create(lv_scr_act());
    lv_obj_set_size(img_gb, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(img_gb, 0, 0);
    lv_obj_set_style_radius(img_gb, 0, 0);
    lv_obj_set_style_border_width(img_gb, 0, 0);
    lv_obj_set_style_bg_color(img_gb, lv_color_hex(0x111111), 0);
#endif

    /* 屏幕顶部状态栏区域 */
    lcd_top_widgets(lv_scr_act());

    /*Create a Tab view object*/
    //tabview_desktop = lv_tabview_create(lv_layer_top(), LV_DIR_TOP, 0);
    tabview_desktop = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 0);
    lv_obj_add_style(tabview_desktop, &style_tabview_desktop, 0);
    lv_obj_add_event_cb(tabview_desktop, get_tab_act_index_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    tab_left = lv_tabview_add_tab(tabview_desktop, "left_desktop");
    tab_main  = lv_tabview_add_tab(tabview_desktop, "main_desktop");
    tab_right = lv_tabview_add_tab(tabview_desktop, "right_desktop");
    lv_tabview_set_act(tabview_desktop, g_tab_act, LV_ANIM_OFF);

	/* 中间图标区域面板 */  
    icon_cont_left = lv_obj_create(tab_left);
    lv_obj_set_size(icon_cont_left, ICON_HOR_RES, ICON_VER_RES);
    lv_obj_center(icon_cont_left);
    lv_obj_add_style(icon_cont_left, &cont_style, 0);
    
    icon_cont_main = lv_obj_create(tab_main);
    lv_obj_set_size(icon_cont_main, ICON_HOR_RES, ICON_VER_RES);
    lv_obj_center(icon_cont_main);
    lv_obj_add_style(icon_cont_main, &cont_style, 0);

    icon_cont_right = lv_obj_create(tab_right);
    lv_obj_set_size(icon_cont_right, ICON_HOR_RES, ICON_VER_RES);
    lv_obj_center(icon_cont_right);
    lv_obj_add_style(icon_cont_right, &cont_style, 0);

    /* 底部快速访问栏面板 */
    lv_obj_t * bottom_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bottom_panel,  LV_PCT(70), 80);
    lv_obj_add_style(bottom_panel, &obj_bottom_panel_style, 0);
    lv_obj_set_layout(bottom_panel, LV_LAYOUT_FLEX);
    //lv_obj_set_style_base_dir(bottom_panel, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(bottom_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bottom_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(bottom_panel, LV_ALIGN_BOTTOM_MID, 0, -15);

    // 菜单位置提示
    set_menu_table_tips(tab_left, 0);
    set_menu_table_tips(tab_main, 1);
    set_menu_table_tips(tab_right, 2);

    for (int i = 0; i < LV_100ASK_MENU_ITEM_COUNT; i++)
    {
        // 分页摆放APP
        if (app_count <= TAB_LEFT_APP_COUNT)
        {
            img_icon = lv_img_create(icon_cont_left);
            label_icon_name = lv_label_create(tab_left);    // 显示在图标下方的app名称
        }
        else if (app_count <= TAB_MAIN_APP_COUNT)
        {
            img_icon = lv_img_create(icon_cont_main);
            label_icon_name = lv_label_create(tab_main);    // 显示在图标下方的app名称
        }
        else if (app_count > TAB_RIGHT_APP_COUNT)
        {
            img_icon = lv_img_create(icon_cont_right);
            label_icon_name = lv_label_create(tab_right);    // 显示在图标下方的app名称
        }    

        //printf("app_count:%d\n", app_count);
        app_count++;
        
        // 图标
        lv_img_set_src(img_icon, app_icon[i]);
        lv_obj_add_flag(img_icon, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_style(img_icon, &icon_style, 0);
        ///lv_obj_add_event_cb(img_icon, event_handler, LV_EVENT_CLICKED, NULL);

        // 底部快速访问栏
        img_bottom_icon = lv_img_create(bottom_panel);
        lv_img_set_src(img_bottom_icon, app_icon[i]);
        //lv_img_set_zoom(img_bottom_icon, 250);
        lv_obj_add_flag(img_bottom_icon, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_style(img_bottom_icon, &icon_style, 0);
        ///lv_obj_add_event_cb(img_bottom_icon, event_handler, LV_EVENT_CLICKED, NULL);

        // 点击图标时调用的服务名(点击图标时父类提取)
        ///strip_ext(de->d_name);  // 去掉最后的后缀名 .png
        label_icon = lv_label_create(img_icon);
        lv_obj_set_width(label_icon, 64);
        lv_label_set_text(label_icon, app_name[i]);

        // 点击图标时调用的服务名(点击图标时父类提取)
        label_bottom_icon = lv_label_create(img_bottom_icon);
        lv_obj_set_width(label_bottom_icon, 64);
        lv_label_set_text(label_bottom_icon, app_name[i]);

        // 显示在图标下方的app名称
        lv_obj_set_style_text_font(label_icon_name, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_icon_name, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_align(label_icon_name, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(label_icon_name, 64);
        lv_label_set_text(label_icon_name, app_name[i]);
        lv_obj_align_to(label_icon_name, img_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        lv_obj_move_foreground(label_icon_name); // 防止被桌面背景图覆盖，将其移到前台           
        //printf("%s\n", de->d_name);
    }
    //lv_obj_move_background(img_gb);  // 将背景移动到后台

}


static void get_tab_act_index_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        if (obj)
        {
            g_tab_act = lv_tabview_get_tab_act(obj);
        }
        else g_tab_act = 1;
    }
}

static void lcd_top_widgets(lv_obj_t * parent)
{
    static lv_style_t obj_layout_style;   // 容器的样式

    lv_style_init(&obj_layout_style);
    lv_style_set_pad_all(&obj_layout_style, 0);
    lv_style_set_bg_opa(&obj_layout_style, 0);
    ///lv_style_set_text_font(&obj_layout_style, &lv_font_montserrat_16);
    lv_style_set_border_opa(&obj_layout_style, 0);
    lv_style_set_radius(&obj_layout_style, 0);
    lv_style_set_text_color(&obj_layout_style, lv_color_hex(0xffffff));

    /* Layout Init */
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_size(panel,  LV_PCT(100), 30);
    lv_obj_add_style(panel, &obj_layout_style, 0);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 5);

    /* 右上角小图标 */
    lv_obj_t * panel_icon = lv_obj_create(panel);
    lv_obj_set_size(panel_icon,  200, 25);
    lv_obj_set_layout(panel_icon, LV_LAYOUT_FLEX);
    lv_obj_set_style_base_dir(panel_icon, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(panel_icon, LV_FLEX_FLOW_ROW);
    lv_obj_align(panel_icon, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(panel_icon, &obj_layout_style, 0);

    lv_obj_t * label = lv_label_create(panel_icon);
    lv_label_set_text(label,  " ");

    lv_obj_t * label_bat = lv_label_create(panel_icon);
    lv_label_set_text(label_bat,  LV_SYMBOL_BATTERY_EMPTY);

    lv_obj_t * label_batchar = lv_label_create(label_bat);
    lv_obj_set_style_text_font(label_batchar, &lv_font_montserrat_14, 0);
    lv_label_set_text(label_batchar,  LV_SYMBOL_CHARGE);
    lv_obj_center(label_batchar);


    lv_obj_t * label_wifi = lv_label_create(panel_icon);
    lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);

    lv_obj_t * label_time = lv_label_create(panel);
    lv_label_set_text(label_time, "  ");
    lv_obj_align(label_time, LV_ALIGN_LEFT_MID, 0, 0);

    ///lv_timer_t * timer = lv_timer_create(lv_timer_update_time, 1000,  label_time);
}

static void set_menu_table_tips(lv_obj_t * parent, int count)
{
    lv_obj_t * round[3];

    /* 左菜单指示 */
    round[0] = lv_obj_create(parent);       // 左边
    lv_obj_set_style_border_opa(round[0], 0, 0);
    lv_obj_set_size(round[0], MENU_TABLE_TIPS_PROMPT, MENU_TABLE_TIPS_PROMPT);
    lv_obj_align(round[0], LV_ALIGN_CENTER, -20, 180);

    round[1] = lv_obj_create(parent);       // 中间
    lv_obj_set_style_border_opa(round[1], 0, 0);
    lv_obj_set_size(round[1], MENU_TABLE_TIPS_PROMPT, MENU_TABLE_TIPS_PROMPT);
    lv_obj_align(round[1], LV_ALIGN_CENTER, 0, 180);

    round[2] = lv_obj_create(parent);       // 右边
    lv_obj_set_style_border_opa(round[2], 0, 0);
    lv_obj_set_size(round[2], MENU_TABLE_TIPS_PROMPT, MENU_TABLE_TIPS_PROMPT);
    lv_obj_align(round[2], LV_ALIGN_CENTER, 20, 180);

    for(int i = 0; i < 3; i++)
    {
        if (i == count)
            lv_obj_set_style_bg_opa(round[i], LV_OPA_80, 0);
        else
            lv_obj_set_style_bg_opa(round[i], LV_OPA_30, 0);
    }

}



static void clean_screen_obj(lv_obj_t * parent)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(parent); i++)
    {
        lv_obj_t * child = lv_obj_get_child(parent, i);
        if (child != lv_scr_act())
            lv_obj_del(child);  // lv_obj_clean
        /*Do something with child*/
    }
}
