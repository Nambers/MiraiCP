# -*- coding: utf-8 -*-
# 提取javap -s的签名到C++语句
if __name__ == '__main__':
    re = ""
    s = """   public static final java.lang.String KSend(java.lang.String, boolean);
    descriptor: (Ljava/lang/String;Z)Ljava/lang/String;

  public static final java.lang.String KRecall(java.lang.String);
    descriptor: (Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KQueryImgUrl(java.lang.String);
    descriptor: (Ljava/lang/String;)Ljava/lang/String;

  public static final void KSendLog(java.lang.String, int);
    descriptor: (Ljava/lang/String;I)V

  public static final java.lang.String KRefreshInfo(java.lang.String);
    descriptor: (Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KUploadImg(java.lang.String, java.lang.String);
    descriptor: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KSendFile(java.lang.String, java.lang.String);
    descriptor: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KRemoteFileInfo(java.lang.String, java.lang.String);
    descriptor: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

  public static final void KSchedule(long, java.lang.String);
    descriptor: (JLjava/lang/String;)V

  public static final java.lang.String KMuteM(int, java.lang.String);
    descriptor: (ILjava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KQueryM(java.lang.String);
    descriptor: (Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KKickM(java.lang.String, java.lang.String);
    descriptor: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KMuteGroup(boolean, java.lang.String);
    descriptor: (ZLjava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KQueryML(java.lang.String);
    descriptor: (Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KQueryBFL(long);
    descriptor: (J)Ljava/lang/String;

  public static final java.lang.String KQueryBGL(long);
    descriptor: (J)Ljava/lang/String;

  public static final java.lang.String KQueryOwner(java.lang.String);
    descriptor: (Ljava/lang/String;)Ljava/lang/String;

  public static final java.lang.String KBuildforward(java.lang.String, long);
    descriptor: (Ljava/lang/String;J)Ljava/lang/String;

  public static final java.lang.String KNfroperation(java.lang.String, boolean);
    descriptor: (Ljava/lang/String;Z)Ljava/lang/String;

  public static final java.lang.String KGioperation(java.lang.String, boolean);
    descriptor: (Ljava/lang/String;Z)Ljava/lang/String;

  public static final void KSendWithQuote(java.lang.String, java.lang.String, java.lang.String);
    descriptor: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V

"""
    s = s.split("  public static final ")
    for a in s:
        aa = a.split("\n")
        if len(aa) < 2 or aa[0] == "" or aa[1] == "":
            continue
        f = aa[0].split(" ")[1].split("(")[0]
        s = aa[1].replace("    descriptor: ", "")
        re += "this->" + f + " = env->GetStaticMethodID(CPP_lib, \"" + f + "\", \"" + s + "\");\n"
    print(re)
