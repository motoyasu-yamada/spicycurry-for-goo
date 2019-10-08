
    /**
     *  アプリケーション
     */
    private static MApplication MAPP;
    /**
     *  キャンバスオブジェクト
     */
    private static C _canvas = new C();
    /**
     *  一般リソース管理情報 
     */
    private static int[]      SRES;
    /**
     *  アイコンリソース情報
     */
    private static int[]      IRES;
    /**
     *  小さいアイコン画像
     */
    private static Image      imgIconS;
    /**
     *  大きいアイコン画像
     */
    private static Image      imgIconL;
    /**
     *  活性化画像
     */
    private static Image      imgAlpha;
    /**
     *  トップ背景画像
     */
    private static Image      imgBack;
    /**
     *  GUI画像
     */
    private static Image      imgGui;
    /**
     *  リソースから読み込んだ文字列
     */
    private static String[]   MSTRINGS;
    /**
     *  現在の折りたたみ開閉状態 
     */
    private static int        FOLD_MODE  = FOLD_MODE_OPEN;
    /**
     *  現在アプリが活性化状態か待受状態かどうか
     */
    private static int        MAPPLICATION_MODE;
    /**
     *  待受け画像
     */
    private static MediaImage     MACHIUKE0;

    /**
     *  システムが用意する自動問合せ周期(分)
     */
#ifdef DEBUG
    private static int[]  POLLIG_TIME_LIST = new int[] {1,2,5,10,20};
#else
    private static int[]  POLLIG_TIME_LIST = new int[] {5,10,15,60,240};
#endif
    /**
     *  現在のポーリング周期
     */
    private static int    CURRENT_POLLING_TIME;

    /**
     *  カレントシーン
     */
    private static int    _currentScene;
    /**
     *  待受画像に指定されているファイルのID
     */
    private static Vector _machiukeFileIDList = new Vector();

    /**
     *  描画用フォント
     */
    private static Font _font = Font.getDefaultFont();

    /**
     *  gooパスポートID
     */
    private static String gooPassportID = "";
    /**
     *  gooパスポートパスワード
     */
    private static String gooPassportPasswd = "";
    /**
     *  各アイコンが可視かどうか？
     */
    private static boolean[] iconVisible;
    /**
     *   UTNで取得するID
     */
    private static String utnid;
    /**
     *  アプリバージョン
     */
    private static String version;

    /**
     *  トップ画面に表示されるメールの着信情報メッセージ
     */
    private static String ArrivalMailCount = "";

    /**
     *  ニュースを取得できなかった場合にNEWSに格納する
     *  実態はnullだが、JavaBlenderのバグでNEWS=nullすると
     *  verification errorがでてしまうので回避策
     */
    private static final String[][] NEWS_NULL = new String[0][0];

    /**
     *  ニュース情報を格納する配列
     *  ニュース数 x 属性(ニュース件名、URL)
     */
    private static String[][] NEWS = NEWS_NULL;

    /**
     *  バージョンアップする必要があるかどうか
     */
    private static boolean requireVersionup;
    /**
     *  バージョンアップ通知を無視するかどうか
     */
    private static boolean ignoreVersionup;
    /**
     *  インフォメーションが存在するかどうか
     */
    private static boolean existsInformation;
    /**
     *  インフォメーションタイトル
     */
    private static String informationTitle;
    /**
     *  インフォメーションURL
     */
    private static String informationURL;

    /**
     *  検索ワード
     */
    private static String searchKeyword = "";

    /**
     *  トップ画面のアイコン/ニュース等の画面左端からの余白
     */
#if !defined(DEVICE_505)
    private static final int spaceFromLeft = 10;
#else
    private static final int spaceFromLeft = 20;
#endif
