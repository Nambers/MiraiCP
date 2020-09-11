/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_example_plugin_CPP_lib */

#ifndef _Included_com_example_plugin_CPP_lib
#define _Included_com_example_plugin_CPP_lib
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    Verify
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_Verify
  (JNIEnv *, jobject);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    PrivateMessage
 * Signature: (JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
  (JNIEnv *, jobject, jlong, jstring, jstring, jstring);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    GroupMessage
 * Signature: (JLjava/lang/String;JLjava/lang/String;Ljava/lang/String;JLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
  (JNIEnv *, jobject, jlong, jstring, jlong, jstring, jstring, jlong, jstring);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    FriendRequest
 * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_plugin_CPP_1lib_FriendRequest
  (JNIEnv *, jobject, jlong, jstring, jstring);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    GroupNameChange
 * Signature: (Ljava/lang/String;Ljava/lang/String;JJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupNameChange
  (JNIEnv *, jobject, jstring, jstring, jlong, jlong);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    GroupMemberLeave
 * Signature: (JZ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMemberLeave
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    GroupMemberJoin
 * Signature: (JZ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMemberJoin
  (JNIEnv *, jobject, jlong, jboolean);

#ifdef __cplusplus
}
#endif
#endif
