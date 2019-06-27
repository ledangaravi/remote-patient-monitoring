package com.frieling.android.Patient24App.newGUI;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import mehdi.sakout.aboutpage.AboutPage;
import com.frieling.android.Patient24App.R;

public class AboutFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        ViewGroup viewGroup = (ViewGroup) inflater.inflate(R.layout.fragment_about, container, false);
        // Inflate the layout for this fragment
        View aboutPage = new AboutPage(getActivity())
                .isRTL(false)
                .setImage(R.drawable.icon_p24_hexagon)
                .setDescription(getActivity().getResources().getString(R.string.about_page_descriptionP24))
//                .addItem(versionElement)
//                .addItem(adsElement)
                .addGroup("Connect with us")
                .addEmail("jf1116@imperial.ac.uk")
                .addGitHub("ledangaravi/remote-patient-monitoring")
                .addPlayStore(getActivity().getResources().getString(R.string.about_page_play_store))
                .create();

        viewGroup.addView(aboutPage);


        return viewGroup;
    }

}
