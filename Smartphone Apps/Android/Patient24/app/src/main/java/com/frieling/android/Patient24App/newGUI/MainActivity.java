package com.frieling.android.Patient24App.newGUI;

import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.nfc.NdefMessage;
import android.nfc.NfcAdapter;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.os.Parcelable;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.frieling.android.Patient24App.bluetooth.BluetoothService;
import com.google.android.material.navigation.NavigationView;

import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.UUID;

import com.frieling.android.Patient24App.R;
import com.frieling.android.Patient24App.profile.BleProfileService;
import com.frieling.android.Patient24App.profile.BleProfileServiceReadyActivity;

import static com.frieling.android.Patient24App.newGUI.sensorData.initSensorDataList;

public class MainActivity
        extends BleProfileServiceReadyActivity<BluetoothService.LocalBinder>
        implements
        NavigationHost,
        NavigationView.OnNavigationItemSelectedListener,
        patientViewFragment.OnFragmentInteractionListener,
        historyFragment.OnListFragmentInteractionListener {

	// Extras that can be passed from NFC (see SplashscreenActivity)
	public static final String EXTRA_APP = "application/vnd.no.nordicsemi.type.app";
	public static final String EXTRA_ADDRESS = "application/vnd.no.nordicsemi.type.address";

	public static final String EXTRA_DEVICE_NAME = "com.frieling.android.Patient24App.extra_device_name";

	private ActionBarDrawerToggle mDrawerToggle;

    private NavigationView nv;


    @Override //graphical initialization usually takes place here
    protected void onCreateView(final Bundle savedInstanceState) {
        setContentView(R.layout.main_activity);

        // ensure that Bluetooth exists
        if (!ensureBLEExists())
            finish();

        /**
         * DRAWER
         * */
        DrawerLayout mDrawerLayout;
        final DrawerLayout drawer = mDrawerLayout = findViewById(R.id.drawer_layout);
        drawer.setDrawerShadow(R.drawable.drawer_shadow, GravityCompat.START);


        // Set the drawer toggle as the DrawerListener
        mDrawerToggle = new ActionBarDrawerToggle(this, mDrawerLayout, R.string.drawer_open, R.string.drawer_close) {
            @Override
            public void onDrawerSlide(final View drawerView, final float slideOffset) {
                // Disable the Hamburger icon animation
                super.onDrawerSlide(drawerView, 0);
            }
            @Override
            public void onDrawerClosed(View DrawerView){
                if (fragment != null) {
                    navigateTo(fragment, false);
                }
            }
        };
        mDrawerToggle.getDrawerArrowDrawable().setColor(getResources().getColor(R.color.white));

        drawer.addDrawerListener(mDrawerToggle);


        nv = findViewById(R.id.navigation_view);
        nv.setNavigationItemSelectedListener(this);


        /**
         * FRAGMENT
         */
        if (savedInstanceState == null) {
            navigateTo(new LiveDashboardFragment(), false);
        }

        configureToolbar();




    }



//    @Override
//    protected void onStart() {
//        super.onStart();
//        String address = "CA:19:FD:A4:2D:E4";
//
//        String[] mac = address.split(":");
//        byte[] byteArray = new byte[6];        // mac.length == 6 bytes
//        for(int i = 0; i < mac.length; i++) {
//            byteArray[i] = Integer.decode("0x" + mac[i]).byteValue();
//        }
//
//
//        final Intent intent = getIntent();
//        if (!isDeviceConnected() && NfcAdapter.ACTION_NDEF_DISCOVERED.equals(intent.getAction())) {
//            final BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
//            final BluetoothDevice device = bluetoothAdapter.getRemoteDevice(byteArray);
//            onDeviceSelected(device, device.getName());
//
//            Log.w("NFC_jonas","connecting to HRM1 via onstart()");
//        }
//    }




    /**
     * Navigate to the given fragment.
     *
     * @param fragment       Fragment to navigate to.
     * @param addToBackstack Whether or not the current fragment should be added to the backstack.
     */
    @Override
    public void navigateTo(Fragment fragment, boolean addToBackstack) {

        int anim_in = slideUp ? R.anim.bottom_up_slide_fragment : R.anim.fade_in_fragment;

        FragmentTransaction transaction =
                getSupportFragmentManager()
                        .beginTransaction()
                        .setCustomAnimations(anim_in, R.anim.fade_out_fragment)
                        .replace(R.id.fragment_container, fragment);

        if (addToBackstack) {
            transaction.addToBackStack(null);
        }

        transaction.commit();
        slideUp=false;
    }

    private void configureToolbar() {
        Toolbar toolbar = findViewById(R.id.app_bar);
        setSupportActionBar(toolbar);
        ActionBar actionbar = getSupportActionBar();
        //actionbar.setHomeAsUpIndicator(R.drawable.ic_action_add);
        actionbar.setDisplayHomeAsUpEnabled(true);
    }


    @Override
     public boolean onNavigationItemSelected(MenuItem item) {
         displaySelectedScreen(item.getItemId());
         return true;
     }

    Fragment fragment = null;
    boolean slideUp = false;
    private void displaySelectedScreen(int itemId) {

            //initializing the fragment object which is selected
            switch (itemId) {
                case R.id.dashboard_page:
                    fragment = new LiveDashboardFragment();
                    getSupportActionBar().setTitle(getResources().getString(R.string.drawer_mainpage_title));
                    slideUp =true;
                    break;
                case R.id.patientView_page:
                    fragment = new patientViewFragment();
                    getSupportActionBar().setTitle(getResources().getString(R.string.drawer_patientview_title));
                    break;
                case R.id.history_page:
                    fragment = new historyFragment();
                    getSupportActionBar().setTitle(getResources().getString(R.string.drawer_history_title));
                    break;
                case R.id.settings_page:
                    fragment = new SettingsFragment();
                    getSupportActionBar().setTitle(getResources().getString(R.string.drawer_settings_title));
                    break;
//                case R.id.help_feedback_page:
//                    getSupportActionBar().setTitle(getResources().getString(R.string.drawer_help_title));
//                    break;
                case R.id.about_page:
                    fragment = new AboutFragment();
                    getSupportActionBar().setTitle(getResources().getString(R.string.drawer_about_title));
                    break;
                default: break;
        }
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
    }


    /** Mandatory implementations
     * */
    @Override
    protected void setDefaultUI() {/* TODO clear your UI*/}
    @Override
    protected int getDefaultDeviceName() { return R.string.template_default_name; }
    @Override
    protected int getAboutTextId() {
        return R.string.template_about_text;
    }


	@Override
	protected void onPostCreate(final Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);
		// Sync the toggle state after onRestoreInstanceState has occurred.
		mDrawerToggle.syncState();
	}

	@Override
	public void onConfigurationChanged(@NonNull final Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		mDrawerToggle.onConfigurationChanged(newConfig);
	}

	@Override
	public boolean onOptionsItemSelected(final MenuItem item) {
		// Pass the event to ActionBarDrawerToggle, if it returns
		// true, then it has handled the app icon touch event
		if (mDrawerToggle.onOptionsItemSelected(item)) {
			return true;
		}

		switch (item.getItemId()) {
            case R.id.bluetooth_connect:
                onConnectClicked();
		}
		return true;
	}

    /**
     * Service - Activity - Fragment Communication
     */
    @Override
    protected void onInitialize(final Bundle savedInstanceState) {
        LocalBroadcastManager.getInstance(this).registerReceiver(mBroadcastReceiver, makeIntentFilter());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mBroadcastReceiver);
    }


    @Override
    protected UUID getFilterUUID() {
        // TODO this method may return the UUID of the service that is required to be in the advertisement packet of a device in order to be listed on the Scanner dialog.
        // If null is returned no filtering is done.
        return  UUID.fromString("0000180D-0000-1000-8000-00805f9b34fb"); // Heart Rate service
    }

    @Override
    protected Class<? extends BleProfileService> getServiceClass() {
        return BluetoothService.class;
    }

    @Override
    protected void onServiceBound(final BluetoothService.LocalBinder binder) {
        // not used
    }

    @Override
    protected void onServiceUnbound() {
        // not used
    }

    @Override
    public void onDeviceDisconnected(final BluetoothDevice device) {
        super.onDeviceDisconnected(device);
        //mBatteryLevelView.setText(R.string.not_available);
        String disconnected = "-";
        onHeartRateReceived(disconnected);
        onBatteryReceived(disconnected);
        onBloodOxReceived(disconnected);
        onStepCountReceived(disconnected);
        onTemperatureReceived(disconnected);

        mViewModel.getCurrentValue().setValue(sensorDataList);
    }


    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(final Context context, final Intent intent) {
            final String action = intent.getAction();
            final BluetoothDevice device = intent.getParcelableExtra(BluetoothService.EXTRA_DEVICE);

            if (BluetoothService.BROADCAST_TEMPLATE_MEASUREMENT.equals(action)) {
                final int value = intent.getIntExtra(BluetoothService.EXTRA_DATA, 0);
                onHeartRateReceived(Integer.toString(value));
            }
             else if (BluetoothService.BROADCAST_BATTERY_LEVEL.equals(action)) {
                final int batteryLevel = intent.getIntExtra(BluetoothService.EXTRA_BATTERY_LEVEL, 0);
                onBatteryReceived(Integer.toString(batteryLevel));
            }

            //JF
            else if (BluetoothService.BROADCAST_HTS_MEASUREMENT.equals(action)) {
                final float value = intent.getFloatExtra(BluetoothService.EXTRA_TEMPERATURE, 0.0f);
                int val = Math.round(value);
                onTemperatureReceived(Integer.toString(val));
            }

            //JF2
            else if (BluetoothService.BROADCAST_STEPCOUNT_MEASUREMENT.equals(action)) {
                final int value = intent.getIntExtra(BluetoothService.EXTRA_STEPCOUNT, 0);
                Log.w("P24", "Received Step count integer: " + value);
                onStepCountReceived(Integer.toString(value));
            }

            //JF3
            else if (BluetoothService.BROADCAST_BLOODOX_MEASUREMENT.equals(action)) {
                final int value = intent.getIntExtra(BluetoothService.EXTRA_BLOODOX, 0);
                Log.w("P24", "Received blood ox integer: " + value);
                onBloodOxReceived(Integer.toString(value));
            }

            mViewModel.getCurrentValue().setValue(sensorDataList);
        }
    };
    //needed in featuresactivity
    private static IntentFilter makeIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothService.BROADCAST_TEMPLATE_MEASUREMENT);
        intentFilter.addAction(BluetoothService.BROADCAST_BATTERY_LEVEL);
        //JF
        intentFilter.addAction(BluetoothService.BROADCAST_HTS_MEASUREMENT);
        //JF2
        intentFilter.addAction(BluetoothService.BROADCAST_STEPCOUNT_MEASUREMENT);
        //JF3
        intentFilter.addAction(BluetoothService.BROADCAST_BLOODOX_MEASUREMENT);
        return intentFilter;
    }

    public List<sensorData> sensorDataList = initSensorDataList();


    private int heartRateIDX = 0;
    private int temperatureIDX  = 1;
    private int bloodOxIDX = 2;
    private int stepIDX = 3;
    private int batteryIDX = 4;


    private void onHeartRateReceived(String newReading){
            sensorDataList.get(heartRateIDX).sensorReading = newReading;

    }
    private void onBatteryReceived(String newReading){
        sensorDataList.get(batteryIDX).sensorReading = newReading;
    }
    private void onTemperatureReceived(String newReading) {
        Log.w("P24", "Received Temperauture: " + newReading);
        sensorDataList.get(temperatureIDX).sensorReading = newReading;
    }
    private void onStepCountReceived(String newReading) {
        Log.w("P24", "Received Step count: " + newReading);
        sensorDataList.get(stepIDX).sensorReading = newReading;
    }
    private void onBloodOxReceived(String newReading) {
        Log.w("P24", "Received blood ox: " + newReading);
        sensorDataList.get(bloodOxIDX).sensorReading = newReading;
    }


    /**patientViewFragment Implementation*/
    @Override
    public void onFragmentInteraction(){
        //you can leave it empty

    }
    /**historyFragment Implementation*/
    @Override
    public void onListFragmentInteraction(historyAlertData item){
        // empty
    }


    /**
     * Parsing the NFC Intent
     * */
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        Log.w("NFC_JONAS", "In  OnNewIntent");

        if (NfcAdapter.ACTION_NDEF_DISCOVERED.equals(intent.getAction())) {
                String deviceName_from_NFC = readFromIntent(intent);
        }
//        if (!isDeviceConnected()) {
            if (!isBLEEnabled()) {
                Log.w("NFC_JONAS", "Should be showing BLEDialog");
                showBLEDialog();
            }
            start_NFC_BLE_Scan(); //TODO pass device name as parameter
        //}
    }


    private String readFromIntent(Intent intent) {

        Parcelable[] rawMsgs = intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);
        NdefMessage[] msgs = null;
        if (rawMsgs != null) {
            msgs = new NdefMessage[rawMsgs.length];
            for (int i = 0; i < rawMsgs.length; i++) {
                msgs[i] = (NdefMessage) rawMsgs[i];
            }
        }

        return buildTagViews(msgs);

    }

    private String buildTagViews(NdefMessage[] msgs) {
        if (msgs == null || msgs.length == 0) return "No Device Name provided by NFC";

        String text = "";
//        String tagId = new String(msgs[0].getRecords()[0].getType());
        byte[] payload = msgs[0].getRecords()[0].getPayload();
        String textEncoding = ((payload[0] & 128) == 0) ? "UTF-8" : "UTF-16"; // Get the Text Encoding
        int languageCodeLength = payload[0] & 0063; // Get the Language Code, e.g. "en"
        // String languageCode = new String(payload, 1, languageCodeLength, "US-ASCII");

        try {
            // Get the Text
            text = new String(payload, languageCodeLength + 1, payload.length - languageCodeLength - 1, textEncoding);
        } catch (UnsupportedEncodingException e) {
            Log.e("UnsupportedEncoding", e.toString());
        }

        Log.w("NFC_JONAS", "Text is: " + text);

        return text;
    }


    /**
     * SOS BUTTON
     * */
    public void onButtonPressed(View view) {
        Log.w("button","here");
        String msg = getResources().getString(R.string.sms_call_help);
        sendSMS(msg);
    }

    /**
     * SMS Permissions
     */
    private void sendSMS(String msg) {

//        // Here, thisActivity is the current activity
//        if (ContextCompat.checkSelfPermission(this,
//                Manifest.permission.SEND_SMS)
//                != PackageManager.PERMISSION_GRANTED) {
//
//            // Permission is not granted
//            // Should we show an explanation?
//            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
//                    Manifest.permission.SEND_SMS)) {
//                // Show an explanation to the user *asynchronously* -- don't block
//                // this thread waiting for the user's response! After the user
//                // sees the explanation, try again to request the permission.
//            } else {
//                // No explanation needed; request the permission
//                ActivityCompat.requestPermissions(this,
//                        new String[]{Manifest.permission.SEND_SMS},
//                        MY_PERMISSIONS_REQUEST_SEND_SMS);
//
//                // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
//                // app-defined int constant. The callback method gets the
//                // result of the request.
//            }
//        } else {
//            SmsManager smgr = SmsManager.getDefault();
//            String MobileNumber = "07936698636";
//            smgr.sendTextMessage(MobileNumber,null,msg,null,null);
//            Toast.makeText(this, R.string.sms_sent, Toast.LENGTH_LONG).show();
//        }

    }

    private static final int MY_PERMISSIONS_REQUEST_SEND_SMS = 1;

//    @Override
//    public void onRequestPermissionsResult(int requestCode,
//                                           String[] permissions, int[] grantResults) {
////        switch (requestCode) {
////            case MY_PERMISSIONS_REQUEST_SEND_SMS: {
////                // If request is cancelled, the result arrays are empty.
////                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
////                    // permission was granted, yay!
////                    SmsManager smgr = SmsManager.getDefault();
////                    String MobileNumber = "07936698636";
////                    String Message = "it worked!";
////                    smgr.sendTextMessage(MobileNumber,null,Message,null,null);
////                    Toast.makeText(this, R.string.no_sms_permission, Toast.LENGTH_LONG).show();
////                } else {
////                    // permission denied, boo! Disable the functionality that depends on this permission.
////                    Toast.makeText(this, R.string.no_sms_permission, Toast.LENGTH_LONG).show();
////                }
////                return;
////            }
////        }
//    }

    private boolean ensureBLEExists() {
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.no_ble, Toast.LENGTH_LONG).show();
            return false;
        }
        return true;
    }
}