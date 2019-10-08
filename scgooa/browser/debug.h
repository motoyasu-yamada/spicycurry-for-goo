/*-----------------------------------------------------------------------------
    Javaプリプロセス用のデバッグライブラリ(トレース/アサート)
    
    $Id: debug.h,v 1.1.1.1 2005/03/04 10:31:15 m_yamada Exp $
    $Revision: 1.1.1.1 $
    $Date: 2005/03/04 10:31:15 $
    $Author: m_yamada $

-------------------------------------------------------------------------------*/

#ifdef DEBUG
    #define DEBUG_ONLY(s) s
    #define TRACE(s) System.out.println(__FILE__ + "(" + __LINE__  + ")" + s)
    #define ASSERT(s) do { if (!(s)) {TRACE("assertion failed.");throw new IllegalArgumentException();} } while(false)
    #define VERIFY(s) ASSERT(s)
#else
    #define DEBUG_ONLY(s)
    #define TRACE(s)   
    #define ASSERT(s)
    #define VERIFY
#endif

#if defined(DEBUG) && defined(RELEASE)
    #error both DEBUG and RELEASE are defined.
#endif
#if !defined(DEBUG) &&!defined(RELEASE)
    #error both DEBUG and RELEASE are not defined.
#endif
