package com.frieling.android.Patient24App.newGUI;

import androidx.recyclerview.widget.RecyclerView;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.frieling.android.Patient24App.R;
import com.frieling.android.Patient24App.newGUI.historyFragment.OnListFragmentInteractionListener;
import com.frieling.android.Patient24App.newGUI.dummy.DummyContent.DummyItem;

import java.util.List;

/**
 * {@link RecyclerView.Adapter} that can display a {@link DummyItem} and makes a call to the
 * specified {@link OnListFragmentInteractionListener}.
 * TODO: Replace the implementation with code for your data type.
 */
public class historyRecyclerViewAdapter extends RecyclerView.Adapter<historyRecyclerViewAdapter.ViewHolder> {

    private final List<historyAlertData> mValues;
    private final OnListFragmentInteractionListener mListener;

    public historyRecyclerViewAdapter(List<historyAlertData> items, OnListFragmentInteractionListener listener) {
        mValues = items;
        mListener = listener;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.fragment_history, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(final ViewHolder holder, int position) {


        holder.categoryView.setText(mValues.get(position).alert_category);
        holder.timeView.setText(mValues.get(position).alert_Time);
        holder.alerttextView.setText(mValues.get(position).alert_Text);

        holder.mView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (null != mListener) {
                    // Notify the active callbacks interface (the activity, if the
                    // fragment is attached to one) that an item has been selected.
                    //mListener.onListFragmentInteraction(holder.mItem);
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return mValues.size();
    }




    /**
     * VIEW HOLDER
     * */

    public class ViewHolder extends RecyclerView.ViewHolder {
        public final View mView;
        public final TextView categoryView;
        public final TextView timeView;
        public final TextView alerttextView;


        public ViewHolder(View view) {
            super(view);
            mView = view;
            categoryView = (TextView) view.findViewById(R.id.alert_category);
            timeView = (TextView) view.findViewById(R.id.alert_time);
            alerttextView = (TextView) view.findViewById(R.id.alert_text);

        }

    }
}
