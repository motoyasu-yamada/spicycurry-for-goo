// #if defined(DEBUG)
//    /**
//        アドレス帳データを検証する
//    */
//    private static void validateAD() {
//        ASSERT(AD_NAME.size() == AD_MAIL.size());
//        ASSERT(AD_YOMI.size() == AD_MAIL.size());
//        ASSERT(AD_TEL.size()  == AD_MAIL.size());
//        ASSERT(AD_MAIL.size() <= MAX_ADDRESS_ENTRY);
//        for (int n = 0;n < AD_MAIL.size();n++) {
//            ASSERT(AD_NAME.elementAt(n) instanceof String);
//            ASSERT(AD_YOMI.elementAt(n) instanceof String);
//            ASSERT(AD_TEL. elementAt(n) instanceof String);
//            ASSERT(AD_MAIL.elementAt(n) instanceof String);
//            ASSERT(((String)AD_NAME.elementAt(n)).length() <= MAX_ADDRESS_NAME_LENGTH);
//            ASSERT(((String)AD_YOMI.elementAt(n)).length() <= MAX_ADDRESS_YOMI_LENGTH);
//            ASSERT(((String)AD_TEL. elementAt(n)).length() <= MAX_ADDRESS_TEL_LENGTH);
//            ASSERT(((String)AD_MAIL.elementAt(n)).length() <= MAX_ADDRESS_MAIL_LENGTH);
//        }
//    }
// 
//    /**
//        メールデータを検証する
//    */
//    private static void validateML() {
//        ASSERT(ML_SUBJECT.size()  == ML_MAIL.size());
//        ASSERT(ML_DATE.size()     == ML_MAIL.size());
//        ASSERT(ML_FOLDERID.size() == ML_MAIL.size());
//        ASSERT(ML_BODY.size()     == ML_MAIL.size());
//        ASSERT(ML_ATTCH.size()    == ML_MAIL.size());
//        ASSERT(ML_PGCOUNT.size()  == ML_MAIL.size());
//        ASSERT(ML_ACCOUNTID.size()== ML_MAIL.size());
//        
//        for (int n = 0;n < ML_MAIL.size();n++) {
//            ASSERT(ML_MAIL.    elementAt(n) instanceof String);
//            ASSERT(ML_SUBJECT. elementAt(n) instanceof String);
//            ASSERT(ML_DATE.    elementAt(n) instanceof String);
//            ASSERT(ML_FOLDERID.elementAt(n) instanceof Integer);
//            ASSERT(ML_ACCOUNTID.elementAt(n) instanceof Integer);
//            ASSERT(ML_SATTACH.elementAt(n) instanceof Integer);
// 
//            if (ML_BODY.    elementAt(n) == null) {
//                ASSERT(ML_BODY.    elementAt(n) == null);
//                ASSERT(ML_PGCOUNT. elementAt(n) == null);
//                ASSERT(ML_ATTCH.   elementAt(n) == null);
//            } else {
//                ASSERT(ML_BODY.    elementAt(n) instanceof String);
//                ASSERT(ML_PGCOUNT. elementAt(n) instanceof Integer);
//                ASSERT(ML_ATTCH.   elementAt(n) instanceof String[]);
//            }
//        }
//    }
// 
// #endif
