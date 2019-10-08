/**
 *  リソースバージョン
 *  リソースを強制バージョンアップさせるには
 *  ここの番号をカウントアップする.
 */
#define RESOURCE_VERSION  23
/**
 *  通信成功ステータス
 */
#define PROTOCOL_STATUS_SUCCESS              '1'
/**
 *  画面管理番号:初期化中
 */
private static final int SCENE_INITING       = 0;
private static final int SCENE_MAIN_MACHIUKE = 1;
private static final int SCENE_MAIN          = 2;
private static final int SCENE_BROWSER       = 7;
private static final int SCENE_IMG_FOLDER    = 13;
private static final int SCENE_IMG_PLAYER    = 14;
private static final int SCENE_ICON_FOLDER   = 20;
private static final int SCENE_RETRIEVE      = 25;
private static final int SCENE_GOOPASSPORTID = 30;


#define SCRATCHPAD_URL  "scratchpad:///0;pos="

/**
*   フォールド状態の管理
*   FOLD_MODE変数に対応
*/
//  フォールドが開いた状態
#define FOLD_MODE_OPEN    1
// フォールドが閉じた状態
#define FOLD_MODE_CLOSE   0

/**
*   待受アプリ状態の管理
*   MAPPLICATION_MODE変数に対応
*/
//  非活性化状態
private static final int MAPPLICATION_MODE_MACHIUKE = 0;
//  活性化状態
private static final int  MAPPLICATION_MODE_ACTIVE  = 1;

// リソースから読込済みの文字列を取得
#define GETMSTR(id) (MSTRINGS[id])

private static final String URL_MAILER_RESOURCE    = "res.mr";
private static final String URL_IMAGE_RESOURCE     = "res.ir";
private static final String URL_IMAGE_MACHIUKE0    = "res.m0.gif";
private static final String URL_BROWSER_PROXY      = "bg/";
private static final String URL_ARRIVAL_MAIL_COUNT = "api.12";
private static final String URL_GET_NEWS           = "api.13";
private static final String URL_GET_INFO           = "api.14";
private static final String URL_GET_NEWS2          = "api.15";
private static final int  DEFAULT_POLLING_TIME        = 240;
private static final int  MAX_BOOKMARK_TITLE_LENGTH   = 16;
private static final int  MAX_BOOKMARK_URL_LENGTH     = 255;

/**
*   @warning  実機では、paint / processEvent / processSystemEvent
*   componentActionには同期をかけない。デッドロック防止の為。
*/
#if defined(DEBUG)
    #define SYNCHRONIZED(lock) synchronized(lock)
#endif
#if defined(RELEASE)
    #define SYNCHRONIZED(lock)
#endif
