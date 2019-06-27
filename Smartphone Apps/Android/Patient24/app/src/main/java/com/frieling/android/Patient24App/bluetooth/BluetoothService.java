/*
 * Copyright (c) 2015, Nordic Semiconductor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.frieling.android.Patient24App.bluetooth;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import java.util.Calendar;

import no.nordicsemi.android.ble.common.profile.ht.TemperatureMeasurementCallback;
import no.nordicsemi.android.log.Logger;
import com.frieling.android.Patient24App.newGUI.MainActivity;
import com.frieling.android.Patient24App.R;
import com.frieling.android.Patient24App.P24Application;
import com.frieling.android.Patient24App.profile.BleProfileService;
import com.frieling.android.Patient24App.profile.LoggableBleManager;

public class BluetoothService extends BleProfileService implements BluetoothManagerCallbacks {
	public static final String BROADCAST_TEMPLATE_MEASUREMENT = "com.frieling.android.Patient24App.template.BROADCAST_MEASUREMENT";
	public static final String EXTRA_DATA = "com.frieling.android.Patient24App.template.EXTRA_DATA";

	public static final String BROADCAST_BATTERY_LEVEL = "com.frieling.android.Patient24App.BROADCAST_BATTERY_LEVEL";
	public static final String EXTRA_BATTERY_LEVEL = "com.frieling.android.Patient24App.EXTRA_BATTERY_LEVEL";

	private final static String ACTION_DISCONNECT = "com.frieling.android.Patient24App.template.ACTION_DISCONNECT";

	private final static int NOTIFICATION_ID = 864;
	private final static int OPEN_ACTIVITY_REQ = 0;
	private final static int DISCONNECT_REQ = 1;

	//JF
	public static final String BROADCAST_HTS_MEASUREMENT = "com.frieling.android.Patient24App.hts.BROADCAST_HTS_MEASUREMENT";
	public static final String EXTRA_TEMPERATURE = "com.frieling.android.Patient24App.hts.EXTRA_TEMPERATURE";
	/** The last received temperature value in Celsius degrees. */
	private Float mTemp;

	//JF2
	public static final String BROADCAST_STEPCOUNT_MEASUREMENT = "com.frieling.android.Patient24App.template.BROADCAST_STEPCOUNT_MEASUREMENT";
	public static final String EXTRA_STEPCOUNT = "com.frieling.android.Patient24App.template.EXTRA_STEPCOUNT";

	//JF3
	public static final String BROADCAST_BLOODOX_MEASUREMENT = "com.frieling.android.Patient24App.template.BROADCAST_BLOODOX_MEASUREMENT";
	public static final String EXTRA_BLOODOX = "com.frieling.android.Patient24App.template.EXTRA_STEPCOUNT";

	private BluetoothManager mManager;

	private final LocalBinder mBinder = new TemplateBinder();

	/**
	 * This local binder is an interface for the bound activity to operate with the sensor.
	 */
	class TemplateBinder extends LocalBinder {
		// TODO Define service API that may be used by a bound Activity

		/**
		 * Sends some important data to the device.
		 *
		 * @param parameter some parameter.
		 */
		public void performAction(final String parameter) {
			mManager.performAction(parameter);
		}

		//JF
		/**
		 * Returns the last received temperature value.
		 *
		 * @return Temperature value in Celsius.
		 */
		Float getTemperature() {
			return mTemp;
		}
	}


	@Override
	protected LocalBinder getBinder() {
		return mBinder;
	}

	@Override
	protected LoggableBleManager<BluetoothManagerCallbacks> initializeManager() {
		return mManager = new BluetoothManager(this);
	}

	@Override
	public void onCreate() {
		super.onCreate();

		final IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_DISCONNECT);
		registerReceiver(mDisconnectActionBroadcastReceiver, filter);
	}

	@Override
	public void onDestroy() {
		// when user has disconnected from the sensor, we have to cancel the notification that we've created some milliseconds before using unbindService
		cancelNotification();
		unregisterReceiver(mDisconnectActionBroadcastReceiver);

		super.onDestroy();
	}

	@Override
	protected void onRebind() {
		// when the activity rebinds to the service, remove the notification
		cancelNotification();
	}

	@Override
	protected void onUnbind() {
		// when the activity closes we need to show the notification that user is connected to the sensor
		createNotification(R.string.template_notification_connected_message, 0);
	}

	@Override
	public void onSampleValueReceived(@NonNull final BluetoothDevice device, final int value) {
		final Intent broadcast = new Intent(BROADCAST_TEMPLATE_MEASUREMENT);
		broadcast.putExtra(EXTRA_DEVICE, getBluetoothDevice());
		broadcast.putExtra(EXTRA_DATA, value);
		LocalBroadcastManager.getInstance(this).sendBroadcast(broadcast);

		if (!mBound) {
			// Here we may update the notification to display the current value.
			// TODO modify the notification here
		}
	}

	@Override
	public void onBatteryLevelChanged(@NonNull final BluetoothDevice device, final int batteryLevel) { //JF TODO
		final Intent broadcast = new Intent(BROADCAST_BATTERY_LEVEL);
		broadcast.putExtra(EXTRA_DEVICE, getBluetoothDevice());
		broadcast.putExtra(EXTRA_BATTERY_LEVEL, batteryLevel);
		LocalBroadcastManager.getInstance(this).sendBroadcast(broadcast);
	}

	//JF
	@Override
	public void onTemperatureMeasurementReceived(@NonNull final BluetoothDevice device,
												 final float temperature, final int unit,
												 @Nullable final Calendar calendar,
												 @Nullable final Integer type) {
		mTemp = TemperatureMeasurementCallback.toCelsius(temperature, unit);

		final Intent broadcast = new Intent(BROADCAST_HTS_MEASUREMENT);
		broadcast.putExtra(EXTRA_DEVICE, getBluetoothDevice());
		broadcast.putExtra(EXTRA_TEMPERATURE, mTemp);
		// ignore the rest
		LocalBroadcastManager.getInstance(this).sendBroadcast(broadcast);

		Log.w("P24", "Broadcasting Temperature: "+ mTemp);

	}

	@Override
	public void onStepCountReceived(final int stepCount){
		final Intent broadcast = new Intent(BROADCAST_STEPCOUNT_MEASUREMENT);
		//broadcast.putExtra(EXTRA_DEVICE, getBluetoothDevice());
		broadcast.putExtra(EXTRA_STEPCOUNT, stepCount);
		LocalBroadcastManager.getInstance(this).sendBroadcast(broadcast);

		Log.w("P24", "Broadcasting Step Count: "+ stepCount);
	}

	@Override
	public void onBloodOxReceived(final int bloodOxValue){
		final Intent broadcast = new Intent(BROADCAST_BLOODOX_MEASUREMENT);
		//broadcast.putExtra(EXTRA_DEVICE, getBluetoothDevice());
		broadcast.putExtra(EXTRA_BLOODOX, bloodOxValue);
		LocalBroadcastManager.getInstance(this).sendBroadcast(broadcast);

		Log.w("P24", "Broadcasting Blood Oxygen: "+ bloodOxValue);
	}


	/**
	 * Creates the notification.
	 *"in onTemperatureMeasurementReceived in BluetoothService"
	 * @param messageResId message resource id. The message must have one String parameter,<br />
	 *                     f.e. <code>&lt;string name="name"&gt;%s is connected&lt;/string&gt;</code>
	 * @param defaults     signals that will be used to notify the user
	 */
	private void createNotification(final int messageResId, final int defaults) {
		final Intent parentIntent = new Intent(this, MainActivity.class);
		parentIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		final Intent targetIntent = new Intent(this, MainActivity.class);

		final Intent disconnect = new Intent(ACTION_DISCONNECT);
		final PendingIntent disconnectAction = PendingIntent.getBroadcast(this, DISCONNECT_REQ, disconnect, PendingIntent.FLAG_UPDATE_CURRENT);

		// both activities above have launchMode="singleTask" in the AndroidManifest.xml file, so if the task is already running, it will be resumed
		final PendingIntent pendingIntent = PendingIntent.getActivities(this, OPEN_ACTIVITY_REQ, new Intent[]{parentIntent, targetIntent}, PendingIntent.FLAG_UPDATE_CURRENT);
		final NotificationCompat.Builder builder = new NotificationCompat.Builder(this, P24Application.CONNECTED_DEVICE_CHANNEL);
		builder.setContentIntent(pendingIntent);
		builder.setContentTitle(getString(R.string.app_name)).setContentText(getString(messageResId, getDeviceName()));
		builder.setSmallIcon(R.drawable.ic_stat_notify_template);
		builder.setShowWhen(defaults != 0).setDefaults(defaults).setAutoCancel(true).setOngoing(true);
		builder.addAction(new NotificationCompat.Action(R.drawable.ic_action_bluetooth, getString(R.string.template_notification_action_disconnect), disconnectAction));

		final Notification notification = builder.build();
		final NotificationManager nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		nm.notify(NOTIFICATION_ID, notification);
	}

	/**
	 * Cancels the existing notification. If there is no active notification this method does nothing
	 */
	private void cancelNotification() {
		final NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		nm.cancel(NOTIFICATION_ID);
	}

	/**
	 * This broadcast receiver listens for {@link #ACTION_DISCONNECT} that may be fired by pressing Disconnect action button on the notification.
	 */
	private final BroadcastReceiver mDisconnectActionBroadcastReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(final Context context, final Intent intent) {
			Logger.i(getLogSession(), "[Notification] Disconnect action pressed");
			if (isConnected())
				getBinder().disconnect();
			else
				stopSelf();
		}
	};
}
