/** $Id: SCMStart.h,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $
    @spec JAMファイルの仕様
        一番目の引数 gooPassportID
        二番目の引数 端末ID
        
    @memo 初期化動作の流れ
        1 - 変数の初期化
        2 o リソースのアップデート
        3 - リソース文字列の読込
        4 o 初期画像の取得
        5 o 初期音声の取得
        6 o リソース画像読込
        7 o マイボックス待受読込
        8 o マイボックス画像読込
        9 o ユーザデータの読込
        
        -  ユーザの進捗を説明しない
        o  ユーザに進捗を説明する
*/

    /**
     *  ティッカーのスクロールカウント
     */
    private static int     _tickerCount;
    /**
     *  ティッカー描画中かどうか
     */
    private static boolean _enableTicker;

    /**
    *   アプリケーションのエントリーポイント
    *   @see IApplication.start
    */
    protected static void start() {
        CURRENT_POLLING_TIME    = DEFAULT_POLLING_TIME;
        _confFontSize           = 1;
        _confDisplayImage       = true;
        changeFont(_confFontSize);

        _currentScene = SCENE_INITING;
        SCM_SCENE_INIT_MSG = SystemMessage.BootingInInit;
        activateCanvas(null,null);
        _canvas.repaint2();

        MAPP  = (MApplication) IApplication.getCurrentApp();
        if (MAPP.getLaunchType()!= IApplication.LAUNCHED_AS_CONCIERGE) {
            MAPPLICATION_MODE = MAPPLICATION_MODE_ACTIVE;
        }
        try {
            //  リソースをサーバーからスクラッチパッドに読み込む
            int previousResVer = installResources(URL_MAILER_RESOURCE,
                                                  RESOURCE_VERSION,
                                                  SPPOS_SRES);
            installResources(URL_IMAGE_RESOURCE,RESOURCE_VERSION,SPPOS_IRES);
            SRES     = loadResources(SPPOS_SRES);
            MSTRINGS = loadStrings  (SRES[IDS]);

            //  初期待受画像と着音声を読み込む
            if (previousResVer == 0) {
                SCM_SCENE_INIT_MSG = SystemMessage.InstallingInInt;
                repaint2();

                //  標準画像のダウンロード
                FileSystem.mountDir(DIRI_IMG);
                FileSystem.writeFile(0,download(URL_IMAGE_MACHIUKE0,null));
                FileSystem.rename(0,SystemMessage.DefaultMachiuke);
                _machiukeFileIDList.addElement(new Integer(0));

                //  初回またはリソース取得時にアイコン全アクティブ設定
                initIconConf();
                saveConf();
            }

            SCM_SCENE_INIT_MSG = SystemMessage.LoadingDataInInit;
            repaint2();
            loadConf();
            loadBookmark();
            loadMachiuke();

            SCM_SCENE_INIT_MSG = SystemMessage.LoadingResInInit;
            repaint2();

            IRES     = loadResources(SPPOS_IRES);
            imgIconS = loadMediaI(IRES[IDB_ICONS]).getImage();
            imgIconL = loadMediaI(IRES[IDB_ICONL]).getImage();
            imgAlpha = loadMediaI(IRES[IDB_ALPHA]).     getImage();
            imgGui   = loadMediaI(IRES[IDB_GUI]).       getImage();
            imgBack  = loadMediaI(IRES[IDB_BACK_IMAGE]).getImage();

            System.out.println("*3");
            changeFont(_confFontSize);

        } catch(Throwable e) {
            e.printStackTrace();
            FileSystem.writeScratchpad(SPPOS_SRES,new byte[2],0,2);
            showDialog(SystemMessage.INSTALL_ERROR,Dialog.DIALOG_ERROR);
            MAPP.terminate();
        }

        //  goo新着メール件数
        SCM_SCENE_INIT_MSG = SystemMessage.LoadingGetMailInInit;
        repaint2();
        String[] args = MAPP.getArgs();
        utnid   = args[0];
        version = args[1];
        protocolArrivalMailCount();

        //  ニュース取得
        SCM_SCENE_INIT_MSG = SystemMessage.LoadingGetNewsInInit;
        repaint2();
        protocolGetNews();

        if (MAPPLICATION_MODE == MAPPLICATION_MODE_ACTIVE) {
            openMain();
        } else {
            ASSERT(MAPPLICATION_MODE == MAPPLICATION_MODE_MACHIUKE);
            openMainMachiuke();
        }

        for (;;) {
            try {
                _processEvent();
            } catch(Throwable t) {
                t.printStackTrace();
                showDialog(t.toString(),Dialog.DIALOG_ERROR);
            }
        }
    }

    /**
     *  ティッカーカウントを取得.
     */
    private static final int getTicker() {
        return _tickerCount;
    }

    /**
     *  ティッカータイマー開始 
     *  描画スレッド / キーイベントスレッド / 待ちうけ状態変更イベントスレッド
     *  等の複数のスレッドからのアクセスが考えられる為同期処理をしています。
     *  同期は _canvas で行っています。
     */
    private static void startTimer() {
        _enableTicker = true;
        _tickerCount = 0;
    }

    /**
     *   ティッカータイマーの停止 
     *   描画スレッド / キーイベントスレッド / 待ちうけ状態変更イベントスレッド
     *   等の複数のスレッドからのアクセスが考えられる為同期処理をしています。
     */
    private static void stopTimer() {
        _enableTicker = false;
    }

    /** スクラッチパッドに保存されているリソースのバージョンが
        requiredVersionと同様のものであればなにもしない。
        requiredVersionと異なるものであれば、
        サーバーからリソースをダウンロードし、
        ダウンロードしたリソースのバージョンをrequiredVersionにマーキングする。
        
        @param   url             リソースを取得するURL
        @param   requiredVersion 最新のリソースバージョン
        @param   scpos           保存するスクラッチパッド位置
        @return  直前にスクラッチパッドに保存されていたリソースバージョン
        
        @exception Exception このメソッドが投げるいかなる例外も*復旧不可能*です。<br> 
                             ユーザに警告の上終了してください。
     */
    private static int installResources(String url,int requiredVersion,int scpos) throws Exception {
        ASSERT(0 <= requiredVersion && requiredVersion <= 0xFFFF);
        ASSERT(0 <= scpos && scpos < MAX_SCRATCHPAD_LENGTH);

        InputStream input = Connector.openInputStream(SCRATCHPAD_URL + scpos);
        int currentVersion = readUI16(input);
        input.close();
        TRACE("Current resource version := " + currentVersion);
        if (currentVersion == requiredVersion) {
            TRACE("Current resource version input latest.");
            return currentVersion;
        }
        byte[] buffer = download(url,null);
        if (buffer == null) {
            throw new Exception(url);
        }

        FileSystem.writeScratchpad(scpos,buffer,0,buffer.length);
        byte[] version = new byte[] {(byte)(requiredVersion & 0xFF),(byte)(requiredVersion >> 8)};
        FileSystem.writeScratchpad(scpos,version,0,2);

        return currentVersion;
    }
