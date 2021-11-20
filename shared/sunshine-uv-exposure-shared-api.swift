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
    //var htmlstringattributed: NSAttributedString
    @Published var htmlstring: String
    
    @Published var authorizationStatus: CLAuthorizationStatus
    
    @Published var vitdtime: String
    @Published var sunburntime: String
    
    @Published var loading: Bool
    @Published var loaded: Bool
    
    @Published public var selected_sky_condition = e_sky_condition.e_sky_cloudless
    @Published public var selected_time_of_day = e_time_of_day_mode.e_time_of_day_around_noon
    @Published public var skin_exposed_percent = 25
    @Published public var chosen_date = Date()
    @Published public var selected_skin_type = 3
    
    private let debug_enabled_flag = false
    
    private let locationManager: CLLocationManager

//    init(_ htmlstring: NSAttributedString) {
//       self.htmlstring = htmlstring
//    }
    
    override init() {
        htmlstring = "nohtmlyet"
        vitdtime = "vitdtime"
        sunburntime = "sunburntime"
        locationManager = CLLocationManager()
        authorizationStatus = locationManager.authorizationStatus
        loading = false
        loaded = false
        
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

    func updateResultsFromHTML(string: String)
    {
//        let pattern = "(?s).*vitamin.*(?<vitd>\\d+:\\d+).*sunburn.*(?<sunburn>\\d+:\\d+)"
        let pattern1 = "(?s).*vitamin.*(\\d+:\\d+).*sunburn.*"
        let pattern2 = "(?s).*vitamin.*sunburn.*(\\d+:\\d+)"
        var part1 = Substring()
        var part2 = Substring()

        do {
            let regex1 = try NSRegularExpression(pattern: pattern1)
            if let match1 = regex1.firstMatch(in: string, range: NSRange(string.startIndex..., in: string)) {
                if let part1range = Range(match1.range(at: 1), in: string) {
                    part1 = string[part1range]
                }
            }
        } catch { print(error) }
        do {
            let regex2 = try NSRegularExpression(pattern: pattern2)
            if let match2 = regex2.firstMatch(in: string, range: NSRange(string.startIndex..., in: string)) {
                if let part2range = Range(match2.range(at: 1), in: string) {
                    part2 = string[part2range]
                }
            }
        } catch { print(error) }
        
        self.htmlstring = string
        if (String(part1).count < 2) {
            self.vitdtime = "Not enough time in the day to get sufficient d3"}
        else {
            self.vitdtime = "Daily d3: " + String(part1)}
        if (String(part2).count < 2) {
            self.sunburntime = "Not enough time in the day to get sunburnt"}
        else {
            self.sunburntime = "Sunburn: " + String(part2)}
        print(part1)
        print(part2)
        self.loaded = true
        self.loading = false
    }
    
    func getExposure()
    {
        self.loaded = false
        self.loading = true
        
        let defaults = UserDefaults.standard
        
        defaults.set(selected_skin_type, forKey: "skin-type")
        defaults.set(selected_time_of_day.rawValue, forKey: "time-of-day")
        defaults.set(selected_sky_condition.rawValue, forKey: "sky-condition-type")
        
        let url = URL(string: "https://fastrt.nilu.no/cgi-bin/olaeng/VitD_quartMEDandMED.cgi")!
        var components = URLComponents(url: url, resolvingAgainstBaseURL: false)!
        
        // date & time math
        let date = selected_time_of_day == e_time_of_day_mode.e_time_of_day_custom_time ? chosen_date : Date()
        var cal = Calendar (identifier: .gregorian)
        cal.timeZone = TimeZone(secondsFromGMT: 0)!
        let dayinyear = Int(cal.ordinality(of: .day, in: .year, for: date) ?? 0)
        let dayofmonth = Int(cal.component(.day, from: date))
        let monthdayofyear = dayinyear - dayofmonth
        let hour_utc = Double(cal.component(.hour, from: date))
        let hour_utc_partial = hour_utc + (Double(cal.component(.minute, from: date)) / 60.0)
        let rounded_hour_utc = round((hour_utc_partial) * 10) / 10.0
        
        // location
        let latitude = locationManager.location?.coordinate.latitude
        let longitude = locationManager.location?.coordinate.longitude
        let roundedlat = round(latitude ?? 0)
        let roundedlong = round(longitude ?? 0)
        let altitude = locationManager.location?.altitude
        let roundedaltitude = round(((altitude ?? 0) / 10)) / 100.0
        
        
        components.queryItems = [
            URLQueryItem(name: "month", value: String(monthdayofyear)),
            URLQueryItem(name: "mday", value: String(dayofmonth)),
            URLQueryItem(name: "city", value: "6"),
            URLQueryItem(name: "location_specification", value: "1"),
            URLQueryItem(name: "latitude", value: String(roundedlat) + "0"),
            URLQueryItem(name: "longitude", value: String(roundedlong) + "0"),
            //URLQueryItem(name: "sza_override", value: "on"),
            URLQueryItem(name: "sza_angle", value: "0"),
            URLQueryItem(name: "skin_index", value: String(self.selected_skin_type)),
            URLQueryItem(name: "exposure_timing", value: self.selected_time_of_day == e_time_of_day_mode.e_time_of_day_around_noon ? "0" : "1"),
            URLQueryItem(name: "start_time", value: String(rounded_hour_utc)),
            URLQueryItem(name: "UVI_flag", value: "0"),
            URLQueryItem(name: "body_exposure", value: String(self.skin_exposed_percent)),
            URLQueryItem(name: "dietary_equivalent", value: "1000"),
            URLQueryItem(name: "sky_condition", value: String(self.selected_sky_condition.rawValue)),
            URLQueryItem(name: "aerosol_specification", value: "0"),
            URLQueryItem(name: "visibility", value: "50"),
            URLQueryItem(name: "angstrom_beta", value: "0.11"),
            URLQueryItem(name: "cloud_fraction", value: "50"),
            URLQueryItem(name: "wc_column1", value: "400"),
            URLQueryItem(name: "wc_column2", value: "400"),
            URLQueryItem(name: "wc_column3", value: "100"),
            URLQueryItem(name: "UVI", value: "3.4"),
            URLQueryItem(name: "ozone_column", value: "350"),
            URLQueryItem(name: "altitude", value: String(roundedaltitude)),
            URLQueryItem(name: "surface", value: "0"),
            URLQueryItem(name: "albedo", value: "0.03"),
            URLQueryItem(name: "type", value: "2")
        ]
        
        let query = components.url!.query

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.httpBody = Data(query!.utf8)
        request.addValue("application/x-www-form-urlencoded", forHTTPHeaderField: "Content-Type")
        
        URLSession(configuration: URLSessionConfiguration.default, delegate: self, delegateQueue: nil).dataTask(with: request) { data, HTTPURLResponse, Error in
            if (data != nil && data?.count != 0) {
                let response = String(data: data!, encoding: .utf8)
                print(response!)
                
                DispatchQueue.main.async {
                    self.updateResultsFromHTML(string: response?.htmlToString ?? "")
                }
            }
        }.resume()
    }
}

extension ContentViewModel: URLSessionDelegate {
    public func urlSession(_ session: URLSession, didReceive challenge: URLAuthenticationChallenge, completionHandler: @escaping (URLSession.AuthChallengeDisposition, URLCredential?) -> Void) {
       //Trust the certificate even if not valid
       completionHandler(.useCredential,
                         URLCredential(trust: challenge.protectionSpace.serverTrust!))
    }
}