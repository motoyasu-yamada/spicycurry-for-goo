#include "configure.h"
#include "imports.h"
#include "debug.h"
#include "html.h"
#include "trace.h"
#include "constants.h"
#include "mrc.h"
#include "mrc2.h"


#if defined(DEBUG) && defined(RELEASE)
    #error DEBUG and RELEASE are defined.
#endif
#if !defined(DEBUG) && !defined(RELEASE)
    #error both DEBUG and RELEASE are not defined.
#endif

/**
 *  Spicycurry
 */
public class A extends MApplication {
    /**
     *  @see MApplication.start
     */
    public void start() {
        C.start();
    }

    /**
     *  @see MApplication.processSystemEvent
     */
    public void processSystemEvent(int type, int param) {
        /// 待受アプリではない ( 旧世代FOMAの障害対応 )
        if (getLaunchType() != LAUNCHED_AS_CONCIERGE) {
            return;
        }
        C.processSystemEvent(type,param);
    }

    public void resume() {
        C.repaint2();
    }
}

#include "C.h"
