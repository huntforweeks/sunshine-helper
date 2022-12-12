//
//  sunshine-uv-exposure-api.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 11/20/21.
//

import Foundation
import CoreLocation

enum e_day_of_year_mode: Int {
    case e_day_of_year_around_noon
    case e_day_of_year_custom_date
}

enum e_time_of_day_mode: Int {
    case e_time_of_day_around_noon
    case e_time_of_day_now
    case e_time_of_day_custom_time
}

enum e_sky_condition: Int {
    case e_sky_cloudless
    case e_sky_scattered
    case e_sky_broken
    case e_sky_overcast
}

extension String {
    var htmlToAttributedString: NSAttributedString? {
        guard let data = data(using: .utf8) else { return nil }
        do {
            return try NSAttributedString(data: data, options: [.documentType: NSAttributedString.DocumentType.html, .characterEncoding:String.Encoding.utf8.rawValue], documentAttributes: nil)
        } catch {
            return nil
        }
    }
    var htmlToString: String {
        return htmlToAttributedString?.string ?? ""
    }
}

class ContentViewModel: NSObject, ObservableObject, CLLocationManagerDelegate {
    @Published var htmlstring: String
    
    @Published var authorizationStatus: CLAuthorizationStatus
    
    @Published var vitdtime: String
    @Published var sunburntime: String
    
    @Published var vitdtimeshort: String
    @Published var sunburntimeshort: String
    
    @Published var sunAngleData: [TimeAngleDataPoint]
    @Published var spectrumDoseData: [WaveLengthDataPoint]
    @Published var erythemaActionSpectrum: [ErythemaActionSpectrumDataPoint]
    @Published var vitaminDActionSpectrum: [VitaminDActionSpectrumDataPoint]
    
    @Published var loading: Bool
    @Published var loaded: Bool
    
    @Published public var selected_sky_condition = e_sky_condition.e_sky_cloudless
    @Published public var selected_time_of_day = e_time_of_day_mode.e_time_of_day_around_noon
    @Published public var skin_exposed_percent = UserDefaults.standard.object(forKey: "body_exposed_percentage") != nil ? UserDefaults.standard.double(forKey: "body_exposed_percentage") : 26.0
    @Published public var chosen_date = Date()
    @Published public var selected_skin_type = 3
    
    @Published public var exposure_result: fastrt_result
    
    @Published public var formattedResultSummary: String
    
    // debug toggles
    var calculateIntegral = true
    var testWebToo = false
    var debugOverrideVals = false
    var debugFastRTParams = false
    var overrideLat = 33.33
    var overridelong = 33.33
    var overrideAlt = 450.0
    
    private let debug_enabled_flag = false
    
    private let locationManager: CLLocationManager
    
    override init() {
        htmlstring = "nohtmlyet"
        vitdtime = "vitdtime"
        sunburntime = "sunburntime"
        vitdtimeshort = "20m"
        sunburntimeshort = "40m"
        locationManager = CLLocationManager()
        authorizationStatus = locationManager.authorizationStatus
        spectrumDoseData = []
        sunAngleData = []
        erythemaActionSpectrum = []
        vitaminDActionSpectrum = []
        loading = false
        loaded = false
        exposure_result = fastrt_result( spectrumDoseData: [])
        formattedResultSummary = ""
        
        let defaults = UserDefaults.standard
        
        if (defaults.object(forKey: "skin-type") != nil) {
            selected_skin_type = defaults.integer(forKey: "skin-type")
        }
        if (defaults.object(forKey: "time-of-day") != nil) {
            selected_time_of_day = e_time_of_day_mode(rawValue: defaults.integer(forKey: "time-of-day"))  ?? e_time_of_day_mode.e_time_of_day_around_noon
        }
        if (defaults.object(forKey: "sky-condition-type") != nil) {
            selected_sky_condition = e_sky_condition(rawValue: defaults.integer(forKey: "sky-condition-type")) ?? e_sky_condition.e_sky_cloudless
        }
        

        super.init()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.startUpdatingLocation()
        
        if (authorizationStatus != .authorizedWhenInUse || authorizationStatus != .authorizedAlways) {
            locationManager.requestWhenInUseAuthorization()
        }
    }
    
    func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        authorizationStatus = manager.authorizationStatus
    }

    func getExposureLocal()
    {
        self.loaded = false
        self.loading = true
        
        let defaults = UserDefaults.standard
        
        defaults.set(selected_skin_type, forKey: "skin-type")
        defaults.set(selected_time_of_day.rawValue, forKey: "time-of-day")
        defaults.set(selected_sky_condition.rawValue, forKey: "sky-condition-type")
        
        
        // date & time math
        let date = selected_time_of_day == e_time_of_day_mode.e_time_of_day_custom_time ?
                chosen_date :
            selected_time_of_day == e_time_of_day_mode.e_time_of_day_around_noon ?
        Calendar.current.date(bySettingHour: 12, minute: 0, second: 0, of: Date())!
            : Date()
        var cal = Calendar (identifier: .gregorian)
        cal.timeZone = TimeZone.current
        let dayinyear = Int(cal.ordinality(of: .day, in: .year, for: date) ?? 0)
        
        cal.timeZone = TimeZone(secondsFromGMT: 0)!
        let seconds_since_midnight = Int((cal.component(.hour, from: date) * 3600) + (cal.component(.minute, from: date) * 60) + cal.component(.second, from: date))
        
        // location
      let latitude = self.debugOverrideVals ? self.overrideLat : locationManager.location?.coordinate.latitude
      let longitude = self.debugOverrideVals ? self.overridelong :  locationManager.location?.coordinate.longitude
        let roundedlat = (latitude ?? 0.0)
        let roundedlong = (longitude ?? 0.0)
      let altitude = self.debugOverrideVals ? self.overrideAlt :  locationManager.location?.altitude
        let roundedaltitude = (altitude ?? 0.0) / 1000.0
                
        let params = fastrt_params(
            exposed_skin: skin_exposed_percent / 100.0,
            fitzpatrick_skin_type: selected_skin_type - 1,
            julianday: dayinyear,
            lat: roundedlat,
            long: roundedlong,
            altitude: roundedaltitude,
            seconds_since_midnight: seconds_since_midnight,
            sky_condition_type: self.selected_sky_condition.rawValue)
        
        
        
        let result = calculateIntegral ? spectrum_helpers.calculateExposureIntergralTimes(
            params: params
        ) : spectrum_helpers.calculateExposureTimes(
            params: params
        )
        sunAngleData = spectrum_helpers.calculateSunAngleData(params: params, fastrtresult: result)
        
        spectrumDoseData = result.spectrumDoseData
        if (erythemaActionSpectrum.isEmpty)
        {
            erythemaActionSpectrum = spectrum_helpers.getErythemaActionSpectrum()
        }
        if (vitaminDActionSpectrum.isEmpty)
        {
            vitaminDActionSpectrum = spectrum_helpers.getVitaminDActionSpectrum()
        }
        
        self.exposure_result = result
        
        let vitaminDDoseTimeSeconds = result.vitdtime
        let erythemaDoseTimeSeconds = result.erythematime
        
        let formatter = DateComponentsFormatter()
        let formattershort = DateComponentsFormatter()
        
        formatter.unitsStyle = .short
        formatter.allowedUnits = [.hour, .minute, .second]
        
        formattershort.unitsStyle = .abbreviated
        formattershort.allowedUnits = [.hour, .minute]
                
        if (result.vitdPercentDose < 1.0) {
            if (result.vitdPercentDose <= 0.0){
                self.vitdtime = "There is too little radiation at the time specified to get your daily dose of D3"}
            else{
                self.vitdtime = "By sunset, you'll only be about \(String(format: "%.1f", result.vitdPercentDose * 100))% of the way to your daily dose of D3"}
            self.vitdtimeshort = "\(String(format: "%.0f", result.vitdPercentDose * 100))%"
        }
        else {
            self.vitdtime = "You'll reach your daily dose of D3 in about " + formatter.string(from: vitaminDDoseTimeSeconds)!
            self.vitdtimeshort = formattershort.string(from: vitaminDDoseTimeSeconds)!
        }
        if (result.erythemaPercentDose < 1.0) {
            if (result.erythemaPercentDose <= 0.0){
                self.sunburntime = "There is too little radiation at the time specified to get a sunburn"}
            else{
                self.sunburntime = "By sunset, you'll only be about \(String(format: "%.1f", result.erythemaPercentDose * 100))% of the way to a sunburn"}
            self.sunburntimeshort = "\(String(format: "%.0f", result.erythemaPercentDose * 100))%"
        }
        else {
            self.sunburntime = "You'll sunburn in about " + formatter.string(from: erythemaDoseTimeSeconds)!
            self.sunburntimeshort = formattershort.string(from: erythemaDoseTimeSeconds)!
        }
        
        formattedResultSummary = getFormattedResultString()
        
        print(self.vitdtime)
        print(self.sunburntime)
        
        self.loaded = true
        self.loading = false
    }
    
    func getExposure()
    {
        getExposureLocal()
    }
    
    func getFormattedResultString() -> String
    {
        var result = ""
        
        let formatter = DateComponentsFormatter()
        
        let dateFormatter = DateFormatter()
        dateFormatter.dateStyle = .long
        dateFormatter.timeStyle = .short
        
        formatter.unitsStyle = .abbreviated
        formatter.allowedUnits = [.hour, .minute, .second]
        var sky_string: String
        switch (selected_sky_condition)
        {
        case .e_sky_cloudless:
            sky_string = "cloudless"
        case .e_sky_scattered:
            sky_string = "scattered"
        case .e_sky_broken:
            sky_string = "broken"
        case .e_sky_overcast:
            sky_string = "overcast"
        }
        let skin_string = "\(selected_skin_type)"
        let date_string = selected_time_of_day == e_time_of_day_mode.e_time_of_day_custom_time ?
        dateFormatter.string(from: chosen_date) :
    selected_time_of_day == e_time_of_day_mode.e_time_of_day_around_noon ?
        dateFormatter.string(from: Calendar.current.date(bySettingHour: 12, minute: 0, second: 0, of: Date())!)
    : dateFormatter.string(from: Date())
        
        result = "\n Based on solar irradiance calculations with the following parameters: \n Percent Skin Exposed \(self.skin_exposed_percent)% \n Skin Type: \(skin_string) \n Cloud conditions \(sky_string) \n at your current location on \(date_string)"
        
        return result
    }
    
    func getResultSiriString() -> String
    {
        var result = ""
        
        let formatter = DateComponentsFormatter()
        
        let dateFormatter = DateFormatter()
        dateFormatter.dateStyle = .long
        dateFormatter.timeStyle = .short
        
        formatter.unitsStyle = .abbreviated
        formatter.allowedUnits = [.hour, .minute, .second]
        var sky_string: String
        switch (selected_sky_condition)
        {
        case .e_sky_cloudless:
            sky_string = "cloudless"
        case .e_sky_scattered:
            sky_string = "scattered"
        case .e_sky_broken:
            sky_string = "broken"
        case .e_sky_overcast:
            sky_string = "overcast"
        }

        let skin_string = "\(selected_skin_type)"
        let date_string = selected_time_of_day == e_time_of_day_mode.e_time_of_day_custom_time ?
        dateFormatter.string(from: chosen_date) :
    selected_time_of_day == e_time_of_day_mode.e_time_of_day_around_noon ?
        dateFormatter.string(from: Calendar.current.date(bySettingHour: 12, minute: 0, second: 0, of: Date())!)
    : dateFormatter.string(from: Date())
        
        result = self.sunburntime + " " + self.vitdtime + "\n Based on solar irradiance calculations with the following parameters: \n Percent Skin Exposed \(self.skin_exposed_percent)% \n Skin Type: \(skin_string) \n Cloud conditions \(sky_string) \n at your current location on \(date_string)"
        
        return result
    }
}
