package com.superbomb.plugins.iap;

import android.app.Activity;
import android.content.Intent;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.util.Log;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClient.BillingResponse;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.superbomb.json.SBJSON;
import com.superbomb.plugins.PluginListener;
import com.superbomb.series.rps.R;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class IAPHelper implements PluginListener, BillingManager.BillingUpdatesListener {

    private static final String TAG = "IAP";

    private static IAPHelper instance = null;
    public static IAPHelper getInstance() {
        if( instance == null ) {
            instance = new IAPHelper();
        }

        return instance;
    }

    private Activity context;
    private BillingManager billingManager;

    private static class Item {
        public String name;
        public String itemId;
        public boolean consumable;

        public Item() {
            consumable = true;
        }

    };
    private final Map<String, Item> items;

    private IAPHelper() {
        items = new HashMap<>();
    }

    public static Activity getContext() {
        return getInstance().context;
    }

    public static BillingManager getBillingManager() {
        return getInstance().billingManager;
    }

    public static Map<String, Item> getItems() {
        return getInstance().items;
    }

    @Override
    public void init(Activity context) {

        this.context = context;
        billingManager = new BillingManager(context, this);
        billingManager.setBase64EncodedPublicKey(context.getString(R.string.iap_base64));

        Log.i(TAG, "init key: " + context.getString(R.string.iap_base64));
    }

    @Override
    public void onStart() {
    }

    @Override
    public void onStop() {
    }

    @Override
    public void onResume() {

        Log.i(TAG, "onResume isReady: " + isReady());

        if( billingManager != null && billingManager.getBillingClientResponseCode() == BillingResponse.OK ) {
            billingManager.queryPurchases();
        }
    }

    @Override
    public void onPause() {
    }

    @Override
    public void onDestroy() {

        if( billingManager != null ) {
            billingManager.destroy();
        }
    }

    @Override
    public boolean onBackPressed() {
        return false;
    }

    @Override
    public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        return false;
    }

    /**
     * IAP 모듈 초기화
     */
    public static void initIAP(String json) {

        Log.i(TAG, "initIAP: " + json);

        Map<String, Object> jsonObj = (Map<String, Object>)SBJSON.parse(json);

        // items
        Map<String, Object> itemsObj = (Map<String, Object>)jsonObj.get("items");
        Map<String, Item> items = getItems();

        Iterator<String> it = itemsObj.keySet().iterator();

        while( it.hasNext() ) {
            String key = it.next();
            Map<String, Object> obj = (Map<String, Object>)itemsObj.get(key);

            Item item = new Item();
            item.name = key;
            item.itemId = (String)obj.get("id");

            if( obj.containsKey("type") ) {
                String type = (String)obj.get("type");
                item.consumable = type.equals("consumable");
            }

            items.put(key, item);
        }

        // base64 key
        // String base64Key = (String)jsonObj.get("key");
        // getBillingManager().setBase64EncodedPublicKey(base64Key);
    }

    @Override
    public void onBillingClientSetupFinished() {

        Log.i(TAG, "onBillingClientSetupFinished");
    }

    @Override
    public void onConsumeFinished(String token, @BillingResponse int responseCode) {

        Log.d(TAG, "Consumption finished. Purchase token: " + token + ", responseCode: " + responseCode);

        // 소모 성공
        if( responseCode == BillingResponse.OK )  {
            // nativeOnPurchased(itemId);
        }
        // 소모 실패
        else {
            nativeOnPurchaseError("consume error. [code : " + responseCode + "]");
        }
    }

    @Override
    public void onPurchasesUpdated(@BillingResponse int responseCode, List<Purchase> purchases) {

        Log.i(TAG, "onPurchasesUpdated responseCode: " + responseCode +
                ", has purchases: " + (purchases != null));

        if( purchases != null ) {
            for( Purchase purchase : purchases ) {
                Log.i(TAG, "purchase: " + purchase);
            }
        }

        // 구매 성공
        if( responseCode == BillingResponse.OK && purchases != null ) {
//            for( Purchase purchase : purchases ) {
//                String itemId = purchase.getSku();
//                Item item = items.get(itemId);
//
//                // 소모성 아이템, 구매 즉시 소모
//                if( item.consumable ) {
//                    billingManager.consumeAsync(purchase.getPurchaseToken());
//                }
//
//                // 구매 완료 리스너 실행
//                nativeOnPurchased(itemId);
//
//                Log.i(TAG, "purchase: " + purchase);
//            }
        }
        // 구매 취소
        else if( responseCode == BillingClient.BillingResponse.USER_CANCELED ) {
            // nativeOnPurchaseCanceled();
        }
        // 구매 실패
        else {
            // Handle any other error codes.
            // nativeOnPurchaseError("purchase error. [code : " + responseCode + "]");
        }

        Log.i(TAG, "end onPurchasesUpdated flow.");
    }

    /**
     * 아이템 결제
     */
    public static void purchase(String itemId, boolean consumable) {

        Log.i(TAG, "purchase itemId: " + itemId);

        Map<String, Item> items = getItems();

        if( !items.containsKey(itemId) ) {
            // 유효하지 않은 아이템
            nativeOnPurchaseError("invalid item");
            return;
        }

        // 결제
        getBillingManager().initiatePurchaseFlow(itemId, BillingClient.SkuType.INAPP);
    }

    /**
     * 아이템 소모
     */
    public static void consume() {

    }

    public static boolean isReady() {
        return getBillingManager() != null && getBillingManager().isBillingClientReady();
    }

    public static native void nativeOnPurchased(String itemId);
    public static native void nativeOnPurchaseError(String errorMsg);
    public static native void nativeOnPurchaseCanceled();
}
