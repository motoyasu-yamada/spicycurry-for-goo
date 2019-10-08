    /**
     *  IME入力待機用オブジェクト
     */
    private static Object wait = new Object();
    /**
     *  ユーザが入力した文字列
     */
    private static String _text;

#if defined(DEVICE_505)
    /**
     *  ユーザにメッセージを入力させる
     *  @param defaultValue デフォルト値
     *  @param constraints 入力形式(TextField.*を参照)
     *  @param maxLength 最大入力文字バイト数
     *  @return キャンセルした場合はnull
     *         それ以外は入力した文字列
     */
    private static final String inputString(String defaultValue, int inputmode,int displaymode,int maxLength) {
        if (maxLength <= 0) {
            maxLength = 10000;
        }
        try {
            synchronized (wait) {
                _canvas.imeOn(defaultValue,displaymode,inputmode);
#if !defined(DEBUG)
                wait.wait();
#endif
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
        clearKeyEvent();
        repaint2();
        return leftByBytes(_text,maxLength);
    }

    /**
     *  ユーザが入力確定/取消したので、
     *  入力を待機しているメインスレッドを再開させる.
     */
    public void processIMEEvent(int type,String text) {
        switch(type) {
        case IME_CANCELED:
            _text = null;
            break;
        case IME_COMMITTED:
            _text = text;
            break;
        default:
            return;
        }
        synchronized (wait) {
            wait.notifyAll();
        }
    }

#else   //  #if defined(DEVICE_505)
    /**
     *  ユーザにメッセージを入力させる
     *  @param defaultValue デフォルト値
     *  @param constraints 入力形式(TextField.*を参照)
     *  @param maxLength 最大入力文字バイト数
     *  @return キャンセルした場合はnull
     *         それ以外は入力した文字列
     */
    private static final String inputString(String value,int inputmode,int display,int maxLength) {
        if (maxLength <= 0 || maxLength > 255) {
            maxLength = 255;
        }

        Panel panel = new Panel();
        TextBox textbox = new TextBox(value,255,2,display);
        textbox.setInputMode(inputmode);
        textbox.setSize(panel.getWidth(),
        panel.getHeight());
        panel.add(textbox);
        textbox.requestFocus();
        Display.setCurrent(panel);
        panel.setComponentListener(_canvas);

        try {
            synchronized (wait) {
                wait.wait();
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }

        panel. setComponentListener(null);
        _text = textbox.getText();
        Display.setCurrent(_canvas);
         clearKeyEvent();
         repaint2();
        return leftByBytes(_text,maxLength);
    }


#endif  //  #if defined(DEVICE_505)

    /**
     *  パネルのコンポーネントのイベント処理
     *  各パネル用の処理メソッドに振り分ける
     *  @see Panel.componentAction
     */
    public void componentAction(Component c,int type,int param) {
        synchronized (wait) {
            wait.notifyAll();
        }
    }
