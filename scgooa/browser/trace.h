/*------------------------------------------------------------------------------
    デバッグ設定オプション
    
    下記をdefineした場合の動作
    
    USE_TRACE_HTML_LOAD     読込/解析時の情報を詳細にトレースする
    USE_TRACE_HTML_FORMAT   整形時の情報を詳細にトレースする
    USE_TRACE_HTML_DRAW     描画時の情報を詳細にトレースする
    USE_TRACE_HTML_OPERATE  操作時(スクロール/選択)の情報を詳細にトレースする
    
    $Id: trace.h,v 1.1.1.1 2005/03/04 10:31:15 m_yamada Exp $
    $Revision: 1.1.1.1 $
    $Date: 2005/03/04 10:31:15 $
    $Author: m_yamada $
-------------------------------------------------------------------------------*/

//#define USE_TRACE_HTML_LOAD
//#define USE_TRACE_HTML_FORMAT
//#define USE_TRACE_HTML_DRAW
//#define USE_TRACE_HTML_OPERATE

// 読込の詳細トレース
#if defined(USE_TRACE_HTML_LOAD)
#define TRACE_HTML_LOAD(s)		TRACE(s)
#else
#define TRACE_HTML_LOAD(s)
#endif

// 整形の詳細トレース
#if defined(USE_TRACE_HTML_FORMAT)
#define TRACE_HTML_FORMAT(s)	TRACE(s)
#else
#define TRACE_HTML_FORMAT(s)
#endif

// 描画の詳細トレース
#if defined(USE_TRACE_HTML_DRAW)
#define TRACE_HTML_DRAW(s)		TRACE(s)
#else
#define TRACE_HTML_DRAW(s)
#endif

// 操作の詳細トレース
#if defined(USE_TRACE_HTML_OPERATE)
#define TRACE_HTML_OPERATE(s) TRACE(s)
#else
#define TRACE_HTML_OPERATE(s)
#endif
