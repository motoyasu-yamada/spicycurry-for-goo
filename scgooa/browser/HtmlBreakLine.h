/**
 *  改行情報 
 *  private class HtmlBreakLine {
 *      int xLine;
 *      int yLine;
 *      int yNext;
 *      int pos;
 *      HtmlBreakLine(int y,int p) {
 *          yLine = y;
 *          pos = p;
 *      }
 *  }
 */
    #define HtmlBreakLine_xLine(v) (v[0])
    #define HtmlBreakLine_yLine(v) (v[1])
    #define HtmlBreakLine_pos(v)   (v[2])
    #define HtmlBreakLine_yNext(v) (v[3])

    #define HtmlBreakLine int[]
    #define newHtmlBreakLine(y,p) new int[]{0,y,p,0}
