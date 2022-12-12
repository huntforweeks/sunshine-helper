//
//  GetSunshineExposureHandler.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 12/10/22.
//

import Foundation

class SunshineStrengthNowHandler: NSObject, SunshineStrengthNowIntentHandling {
    
    func handle(intent: SunshineStrengthNowIntent, completion: @escaping (SunshineStrengthNowIntentResponse) -> Void) {
        let sunshineSharedApi = ContentViewModel()
        sunshineSharedApi.selected_time_of_day = e_time_of_day_mode.e_time_of_day_now
        sunshineSharedApi.getExposure()
        	
        completion(SunshineStrengthNowIntentResponse.success(result_string: sunshineSharedApi.getResultSiriString()))
    }
}
