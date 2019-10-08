    private static class HtmlNode {
        /**
         *  ドキュメント構造:ノード形式
         */
        protected int type;
        /**
         *  整形情報 INPUT/SELECT/TEXTAREAのみ
         */
        protected int width;
        /**
         *  整形情報 INPUT/SELECT/TEXTAREAのみ
         */
        protected int height;
        /**
         *  整形情報: - インタラクティブ要素の垂直位置
         */
        protected int y;
        /**
         *  ドキュメント構造:属性値集合 - タグノードのみ
         */
        protected Hashtable attributes = new Hashtable();
        /**
         *  ドキュメント構造:インタラクティブノードかどうか
         */
        protected boolean isInteractive;
        /**
         *  子ノード - タグノードのみ
         */
        protected Vector childNodes = new Vector();
        /**
         *  整形情報:  改行情報
         */
        protected Vector breaks;

        /* --- 特定ノードのみの情報 --- */
        /**
         *  ドキュメント構造:要素 - テキストノードのみ
         */
        protected String text = new String();
        /**
         *  操作情報 INPUT/SELECT/TEXTAREAのみ
         */
        protected HtmlNode parentForm;
        /**
         *  操作情報 FORMの各子要素の現在の値
         */
        protected Hashtable values;
        /**
         *  操作情報 FORMの各子要素の元の値
         */
        protected Hashtable valuesOrg;
    }
