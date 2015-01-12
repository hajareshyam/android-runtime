package com.tns.internal;

public interface AppBuilderCallback
{
	void onConfigurationChanged(android.content.Context context, android.content.res.Configuration newConfig);
	void onCreate(android.content.Context context);
	void onLowMemory(android.content.Context context);
	void onTerminate(android.content.Context context);
	void onTrimMemory(android.content.Context context, int level);
	Thread.UncaughtExceptionHandler getDefaultUncaughtExceptionHandler();
	ExtractPolicy getExtractPolicy();
}
