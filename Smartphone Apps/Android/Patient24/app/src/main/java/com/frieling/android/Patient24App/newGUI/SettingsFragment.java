package com.frieling.android.Patient24App.newGUI;

import android.app.Activity;
import android.os.Bundle;
import android.text.InputType;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.ActivityChooserView;
import androidx.appcompat.widget.Toolbar;
import androidx.preference.EditTextPreference;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;

import androidx.appcompat.widget.Toolbar;
import androidx.appcompat.app.ActionBar;

import com.frieling.android.Patient24App.R;

public class SettingsFragment extends PreferenceFragmentCompat {


    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.preferences, rootKey);

        /**
         * Edit Text for Phone Number
         */
        EditTextPreference phonePreference = findPreference("PhoneNumber");

        if (phonePreference != null) {
            phonePreference.setSummaryProvider(new Preference.SummaryProvider<EditTextPreference>() {
                @Override
                public CharSequence provideSummary(EditTextPreference preference) {
                    String text = preference.getText();
                    if (TextUtils.isEmpty(text)) {
                        return "Not set";
                    }
                    return "Phone Number: " + text;
                }
            });
            phonePreference.setOnBindEditTextListener(
                    new EditTextPreference.OnBindEditTextListener() {
                        @Override
                        public void onBindEditText(@NonNull EditText editText) {
                            editText.setInputType(InputType.TYPE_CLASS_PHONE);
                        }
                    });
        }

        /**
         * Selecting Dialog for Regular Updates
         */
        ListPreference updaterPreference = findPreference("updater_preference");

        if (updaterPreference != null) {
            updaterPreference.setSummaryProvider(new Preference.SummaryProvider<ListPreference>() {
                @Override
                public CharSequence provideSummary(ListPreference preference) {
                    String text =  (String) preference.getEntry();
                    if (TextUtils.isEmpty(text)) {
                        return "Keep your loved one up to date";
                    }
                    return "SMS update every " + text;
                }
            });
        }

    }

}
