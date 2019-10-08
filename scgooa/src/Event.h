    /**
     *  イベントキューイング&ディスパッチライブラリ
     *
     *
     */

    /**
     *  システムイベント：活性化イベント
     */
    private final static int EVENT_SYS_MODE_CHANGED  = 0x100;   //  EVENT_SYS + 0
    /**
     *  システムイベント：ウエークアップタイマ
     */
    private final static int EVENT_SYS_WAKEUP_TIMER  = 0x101;   //  EVENT_SYS + 1
    /**
     *  システムイベント：携帯折りたたみ開閉
     */
    private final static int EVENT_SYS_FOLD_CHANGED  = 0x102;   //  EVENT_SYS + 2
    /**
     *  システムイベント：再描画
     */
    private final static int EVENT_SYS_REPAINT       = 0x103;

    /**
     *  キーを継続押下中かどうか
     */
    private static boolean _keyRepeating;

    /**
     *  キーイベントは発生していない
     */
    private final static int EVENT_NULL          = -1;
    /**
     *  タッチパネルポインタがクリックされた
     */
    private final static int EVENT_POINTER_SELECTED = 32;
    /**
     *  キーイベントキュー
     *  -1の場合はキーイベントなし
     */
    private static int _keyEvent = EVENT_NULL;
    /**
     *  システムイベントキュー
     */
    private static Vector _sysEvent = new Vector();

    /**
     *  キーイベントを消去するかどうか
     */
    private static boolean _clearKeyEvent;



    /**
     *   待ちうけイベント / 折りたたみ開閉 / ウエークアップ
     *
     *   @warning,@knowhow processSystemEvent を synchronizedすると
     *   下記で CLOSE/OPENのスレッドがデッドロックするようです
     *
     *   @spec フォールドを閉じた時の動作
     *         そのままの画面で、deactivate->sleepする
     *   @spec フォールドを開いた時に動作 (休眠→待受時)
     *         タイマーを復旧させる -> Canvas.processEvent VM_UPDATE_EVENT
     *   @sepc フォールドを開いた時に動作 (活性化時)
     *         何もしない
     *   @spec 活性化ボタンで活性化した場合の動作
     *         ウエルカム音声を再生する
     */
    static void processSystemEvent(int type, int param) {
        int event = EVENT_NULL;
        switch (type) {
        case MApplication.FOLD_CHANGED_EVENT:
            FOLD_MODE = param;
            event = EVENT_SYS_FOLD_CHANGED;
           break;
        case MApplication.MODE_CHANGED_EVENT:
            MAPPLICATION_MODE = MAPPLICATION_MODE_ACTIVE;
            event = EVENT_SYS_MODE_CHANGED;
            break;
        case MApplication.WAKEUP_TIMER_EVENT:
            event = EVENT_SYS_WAKEUP_TIMER;
            break;
        }
        if (event != EVENT_NULL) {
            synchronized(_sysEvent) {
                _sysEvent.addElement(new Integer(event));
            }
        }
    }

    /**
     *  イベントをイベントキューに挿入する.
     *  @see    Canvas#processEvent
     */
    public void processEvent(int type,int param) {
        if (type == Display.KEY_PRESSED_EVENT) {
            _keyEvent = param;
            _keyRepeating = false;
            _clearKeyEvent     = false;
            return;
        }
        if (type == Display.KEY_RELEASED_EVENT) {
            _clearKeyEvent = true;
            return;
        }
    }

    /**
     *  キーイベントを取得
     */
    private static final int keyEvent() {
        int key = _keyEvent;
        if (_clearKeyEvent) {
            _keyEvent = EVENT_NULL;
            _clearKeyEvent = false;
        } else if (_canvas.getKeypadState() == 0) {
            _clearKeyEvent = true;
        }
        return key;
    }

    /**
     *  キーイベントをクリア
     */
    private static final void clearKeyEvent() {
        _keyEvent     = EVENT_NULL;
        _keyRepeating = false;
    }

    /**
     *  キーを換算に話すまで
     */
    private static final void waitKeyRelease() {
        while (keyEvent() != EVENT_NULL) {
        }
        clearKeyEvent();
    }

    /**
     *  イベントのクリアを待機する
     *  @param wait
     */
    private static final void repeatKey() {
        long start = System.currentTimeMillis();
        long limit = start + (_keyRepeating ? 100 : 500);
        for(;;) {
            if (_keyEvent == EVENT_NULL) {
                _keyRepeating = false;
                break;
            }
            if (_clearKeyEvent) {
                _keyEvent = EVENT_NULL;
                _keyRepeating = false;
                break;
            }
            if (limit < System.currentTimeMillis()) {
                _keyRepeating = true;
                break;
            }
            sleepThread(10);
        }
    }
