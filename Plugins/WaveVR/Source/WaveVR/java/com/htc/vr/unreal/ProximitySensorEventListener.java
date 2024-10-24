// "WaveVR SDK
// © 2024 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

package com.htc.vr.unreal;

import android.util.Log;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

public class ProximitySensorEventListener implements SensorEventListener
{
	private static final String TAG = "ProximitySensorEventListener";
	public native void OnHMDWornStateChanged(boolean isWearing);

	@Override
	public void onSensorChanged(SensorEvent event) {
		Log.d(TAG, "onSensorChanged : " + event.toString());
		boolean isWearing = event.values[0] < 1.0f;
		OnHMDWornStateChanged(isWearing);
	}
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {}
}
