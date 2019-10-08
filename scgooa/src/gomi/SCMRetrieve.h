/**
 *  SCMRetrieve     2004/09/13
 *  @author T.sawahashi
 *  @warning ONLY_504
 *  goo検索画面の表示
 *  
 */


    /**
     *  検索ワードが入るテキストボックス
     */
    private static TextBox _retrTextBox;
    /**
     *  検索ボタン
     */
    private static Button _retrButton;
    /**
     *  戻るボタン
     */
    private static Button _cancelButton;
    /**
     *  戻り先画面
     */
    private static int _retrReturnScene;
    /**
     *  検索画面
     */
    private static Panel _retrPanel;

    /**
    *   検索画面を開く
    *   @param 
    *   @param 
    */
    private static void openRetrieve(int attachFileId) {
        
        int color = Graphics.getColorOfRGB(0xcc,0xff,0xff);
        int panelWidth;
        
        _currentScene = SCENE_RETRIEVE;
        
        ASSERT(_retrPanel == null);
        ASSERT(_retrTextBox == null);
        ASSERT(_retrButton == null);
        ASSERT(_cancelButton == null);
        
        _retrPanel = new Panel();
        panelWidth = _retrPanel.getWidth() - PANEL_SCREEN_MARGIN;
        _retrPanel.setTitle(GETMSTR(IDS_TIT_RETRIEVE));
        _retrPanel.setBackground(color);
        
        _retrTextBox = new TextBox("", 30, 1, TextBox.DISPLAY_ANY);
        _retrTextBox.setSize(panelWidth, _retrTextBox.getHeight());
        _retrButton = new Button("検索");
        _retrButton.setSize(panelWidth / 2, _retrButton.getHeight());
        _cancelButton = new Button("ｷｬﾝｾﾙ");
        _cancelButton.setSize(panelWidth / 2, _cancelButton.getHeight());
        _retrPanel.add(_retrTextBox);
        _retrPanel.add(_retrButton);
        _retrPanel.add(_cancelButton);
        _retrTextBox.requestFocus();
        
        openRetrieve();
    }
    
    private static void openRetrieve() {
        ASSERT(_retrPanel != null);
        _retrPanel.setComponentListener(_canvas);
        Display.setCurrent(_retrPanel);
        clearKeyEvent();
    }
    
    /**
    *   検索時のコンポーネントアクション
    */
    private static void componentAction_Retrieve(Component c, int type) {
        String words;
        
        if (c == _retrButton) {
            words = _retrTextBox.getText();
System.out.println("words=" + words);
            _retrReturnScene = SCENE_BROWSER;
            exitRetrieve(words);
        } else if (c == _cancelButton) {
            _retrReturnScene = SCENE_MAIN;
            exitRetrieve("");
        }
        
        return;
    }
    
    /**
    *   検索画面の終了
    *   戻り画面種別
    *       
    */
    private static void exitRetrieve(String strRetrieve) {
        
        _retrPanel      = null;
        _retrButton     = null;
        _cancelButton   = null;
        _retrTextBox    = null;
        

        int returnScene = _retrReturnScene;
        if (returnScene == SCENE_BROWSER) {
            System.out.println("Search Browser Open !!");

            openBrowser(BROWSER_MODE_STANDARD,"http://mobile.goo.ne.jp/search_i.jsp?MT="+ URLEncoder.encode(strRetrieve) +"&i_search=1&AB=yes&IE=sjis&DC=5");
            
        } else if (returnScene == SCENE_MAIN) {
            openMain();
        }

        return;
    }


