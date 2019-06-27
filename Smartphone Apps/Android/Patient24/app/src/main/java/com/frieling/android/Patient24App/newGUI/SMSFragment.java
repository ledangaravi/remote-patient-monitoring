package com.frieling.android.Patient24App.newGUI;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import com.frieling.android.Patient24App.R;

public class SMSFragment extends Fragment {


    private static final int MY_PERMISSIONS_REQUEST_SEND_SMS = 1;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.sms_fragment, container, false);

        // Set up the toolbar
        setUpToolbar(view);

        /**
         * SMS Button
         * */
        final Button button = view.findViewById(R.id.sms_button);
        button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Code here executes on main thread after user presses button
                sendSMS();
            }
        });
        return view;
    }

    /**
     * SMS Permissions
     */
    private void sendSMS() {

        // Here, thisActivity is the current activity
        if (ContextCompat.checkSelfPermission(getActivity(),
                Manifest.permission.SEND_SMS)
                != PackageManager.PERMISSION_GRANTED) {

            // Permission is not granted
            // Should we show an explanation?
            if (ActivityCompat.shouldShowRequestPermissionRationale(getActivity(),
                    Manifest.permission.SEND_SMS)) {
                // Show an explanation to the user *asynchronously* -- don't block
                // this thread waiting for the user's response! After the user
                // sees the explanation, try again to request the permission.
            } else {
                // No explanation needed; request the permission
                ActivityCompat.requestPermissions(getActivity(),
                        new String[]{Manifest.permission.SEND_SMS},
                        MY_PERMISSIONS_REQUEST_SEND_SMS);

                // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
                // app-defined int constant. The callback method gets the
                // result of the request.
            }
        } else {
            SmsManager smgr = SmsManager.getDefault();
            String MobileNumber = "07936698636";
            String Message = "it worked!";
            smgr.sendTextMessage(MobileNumber,null,Message,null,null);
            Toast.makeText(getActivity(), R.string.sms_sent, Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_SEND_SMS: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay!
                    SmsManager smgr = SmsManager.getDefault();
                    String MobileNumber = "07936698636";
                    String Message = "it worked!";
                    smgr.sendTextMessage(MobileNumber,null,Message,null,null);
                    Toast.makeText(getActivity(), R.string.no_sms_permission, Toast.LENGTH_LONG).show();
                } else {
                    // permission denied, boo! Disable the functionality that depends on this permission.
                    Toast.makeText(getActivity(), R.string.no_sms_permission, Toast.LENGTH_LONG).show();
                }
                return;
            }
        }
    }
    private void setUpToolbar(View view) {
        Toolbar toolbar = view.findViewById(R.id.app_bar);
        AppCompatActivity activity = (AppCompatActivity) getActivity();
        if (activity != null) {
            activity.setSupportActionBar(toolbar);
        }
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater menuInflater) {
        menuInflater.inflate(R.menu.settings_and_about, menu);
        super.onCreateOptionsMenu(menu, menuInflater);
    }

}
