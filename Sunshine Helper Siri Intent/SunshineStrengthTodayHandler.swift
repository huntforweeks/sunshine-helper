//
//  SunshineStrengthTodayHandler.swift
//  Sunshine Helper Siri Intent
//
//  Created by Hunter Weeks on 12/11/22.
//

import Foundation

class SunshineStrengthTodayHandler: NSObject, SunshineStrengthTodayIntentHandling {
    
    func handle(intent: SunshineStrengthTodayIntent, completion: @escaping (SunshineStrengthTodayIntentResponse) -> Void) {
        let sunshineSharedApi = ContentViewModel()
        sunshineSharedApi.selected_time_of_day = e_time_of_day_mode.e_time_of_day_around_noon
        sunshineSharedApi.getExposure()
            
        completion(SunshineStrengthTodayIntentResponse.success(result_string: sunshineSharedApi.getResultSiriString()))
    }
}
