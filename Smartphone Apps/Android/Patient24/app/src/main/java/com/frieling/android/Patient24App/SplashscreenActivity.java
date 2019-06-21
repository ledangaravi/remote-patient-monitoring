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
package com.frieling.android.Patient24App;

import android.app.Activity;
import android.content.Intent;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Parcelable;
import android.util.Log;

import com.frieling.android.Patient24App.newGUI.MainActivity;

import java.io.UnsupportedEncodingException;

public class SplashscreenActivity extends Activity {
	/** Splash screen duration time in milliseconds */
	private static final int DELAY = 1000;

	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_splashscreen);

		Log.w("NFC_JONAS", "I AM IN THE SPLASH-SCREEN");


		final Intent intent = getIntent();

		if (NfcAdapter.ACTION_NDEF_DISCOVERED.equals(intent.getAction())) {
			Log.w("NFC_JONAS", "in iF");
		}
		else
			Log.w("NFC_JONAS", "Intent is: " + intent.getAction());

		// Jump to SensorsActivity after DELAY milliseconds
		new Handler().postDelayed(() -> {

			//New Intent to pass message to MainActivity
			final Intent newIntent = new Intent(SplashscreenActivity.this, MainActivity.class);
			newIntent.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);

			// Handle NFC message, if app was opened using NFC AAR record

//			if (NfcAdapter.ACTION_NDEF_DISCOVERED.equals(intent.getAction())) {
//
//				Log.w("NFC_JONAS", "NFC_MESSAGE RECEIVED");
//
//				String deviceName = readFromIntent(intent);
//				newIntent.putExtra(MainActivity.EXTRA_DEVICE_NAME, deviceName);
//			}
			startActivity(newIntent);
			finish();
		}, DELAY);
	}

	@Override
	public void onBackPressed() {
		// do nothing. Protect from exiting the application when splash screen is shown
	}

	/**
	 * Inverts endianness of the byte array.
	 * @param bytes input byte array
	 * @return byte array in opposite order
	 */
	private byte[] invertEndianness(final byte[] bytes) {
		if (bytes == null)
			return null;
		final int length = bytes.length;
		final byte[] result = new byte[length];
		for (int i = 0; i < length; i++)
			result[i] = bytes[length - i - 1];
		return result;
	}

//
//	private String readFromIntent(Intent intent) {
//
//		Parcelable[] rawMsgs = intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);
//		NdefMessage[] msgs = null;
//		if (rawMsgs != null) {
//			msgs = new NdefMessage[rawMsgs.length];
//			for (int i = 0; i < rawMsgs.length; i++) {
//				msgs[i] = (NdefMessage) rawMsgs[i];
//			}
//		}
//
//		return buildTagViews(msgs);
//
//	}
//
//	private String buildTagViews(NdefMessage[] msgs) {
//		if (msgs == null || msgs.length == 0) return "No Device Name provided by NFC";
//
//		String text = "";
////        String tagId = new String(msgs[0].getRecords()[0].getType());
//		byte[] payload = msgs[0].getRecords()[0].getPayload();
//		String textEncoding = ((payload[0] & 128) == 0) ? "UTF-8" : "UTF-16"; // Get the Text Encoding
//		int languageCodeLength = payload[0] & 0063; // Get the Language Code, e.g. "en"
//		// String languageCode = new String(payload, 1, languageCodeLength, "US-ASCII");
//
//		try {
//			// Get the Text
//			text = new String(payload, languageCodeLength + 1, payload.length - languageCodeLength - 1, textEncoding);
//		} catch (UnsupportedEncodingException e) {
//			Log.e("UnsupportedEncoding", e.toString());
//		}
//
//		Log.w("NFC_JONAS", "Text is: " + text);
//
//		return text;
//	}
}
