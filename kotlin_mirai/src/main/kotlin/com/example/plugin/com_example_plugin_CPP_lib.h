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
 * Signature: (JLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    GroupMessage
 * Signature: (JJLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
  (JNIEnv *, jobject, jlong, jlong, jstring);

/*
 * Class:     com_example_plugin_CPP_lib
 * Method:    FriendRequest
 * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_plugin_CPP_1lib_FriendRequest
  (JNIEnv *, jobject, jlong, jstring, jstring);

#ifdef __cplusplus
}
#endif
#endif
