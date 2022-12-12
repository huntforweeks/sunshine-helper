import Foundation
import FastRT
import SwiftUICharts
import SwiftUI

struct fastrt_params {
    var exposed_skin = 1.0
    var fitzpatrick_skin_type = 1
    var julianday = 104
    var lat = 0.0
    var long = 0.0
    var altitude = 0.0
    var seconds_since_midnight = 32400
    var sky_condition_type = 0
}

struct TimeAngleDataPoint {
    var x: Double
    var y: Double
    var vitd: Bool // true if time is after start period, and before desired amount for d3
    var erythema: Bool // true if time is after start period and before sunburn is acquired
    
    func getDataPoint() -> DataPoint
    {
        let legend = vitd && erythema ? Legend(color: .green, label: "Required Time for Daily Vitamin D3", order: 2)
        : vitd && !erythema ? Legend(color: .red, label: "Portion of time for Daily D3 beyond sunburn threshold, consider exposing more skin", order: 4)
        : erythema ? Legend(color: .blue, label: "Time before sunburn", order: 3)
        : Legend(color: .yellow, label: "Sun UV Intensity over the day", order: 1)
        
        let formatter = DateComponentsFormatter()
        formatter.allowedUnits = [.hour, .minute]
        let label = formatter.string(from: TimeInterval(x))!
//        let label = ""
        let val = !self.y.isNaN && self.y != Double.infinity ? self.y : 0.0
        return DataPoint(startValue: 0.0, endValue: val, label: LocalizedStringKey.init(label), legend: legend)
    }
}

struct WaveLengthDataPoint {
    var x: Double
    var y: Double

    func getDataPoint() -> DataPoint
    {
        let sunLegend =  Legend(color: .purple, label: "UV Exposure", order: 1)
        let label = String(x) + "nm"
        let val = !self.y.isNaN && self.y != Double.infinity && self.y != 0.0 ? log10(self.y * 100  + 1): 0.0
        return DataPoint(startValue: 0.0, endValue: val, label: LocalizedStringKey.init(label), legend: sunLegend)
    }
}

struct ErythemaActionSpectrumDataPoint {
    var x: Double
    var y: Double

    func getDataPoint() -> DataPoint
    {
        let sunLegend =  Legend(color: .red, label: "UV Spectrum Weighting for sunburn", order: 1)
        let label = String(x) + "nm"
        let val = !self.y.isNaN && self.y != Double.infinity && self.y != 0.0 ? log10(self.y * 5000 + 1) : 0.0
        return DataPoint(startValue: 0.0, endValue: val, label: LocalizedStringKey.init(label), legend: sunLegend)
    }
}

struct VitaminDActionSpectrumDataPoint {
    var x: Double
    var y: Double

    func getDataPoint() -> DataPoint
    {
        let sunLegend =  Legend(color: .green, label: "UV Spectrum Weighting for D3", order: 1)
        let label = String(x) + "nm"
        let val = !self.y.isNaN && self.y != Double.infinity && self.y != 0.0 ? log10(self.y * 5000 + 1)  : 0.0
        return DataPoint(startValue: 0.0, endValue: val, label: LocalizedStringKey.init(label), legend: sunLegend)
    }
}

struct fastrt_result {
    var vitdtime = 0.0
    var erythematime =  0.0
    var vitdPercentDose = 0.0
    var erythemaPercentDose = 0.0
    var spectrumDoseData : [WaveLengthDataPoint]
}

class spectrum_helpers {
    static let startWavelength = Int32(290);
    static let endWavelength = Int32(400);
    static let stepWavelength = 1.0;

    static func getSunriseSunset(params: fastrt_params) -> (Int, Int)
    {
        var sunrise = Int32(-1)
        var sunset = Int32(-1)
        (FastRT.get_sunrise_sunset(&sunrise, &sunset, Int32(params.julianday), params.lat, params.long, params.altitude))
            
        return (Int(sunrise), Int(sunset))
    }
    
    static func getErythemaActionSpectrum() -> [ErythemaActionSpectrumDataPoint]
    {
        var result : [ErythemaActionSpectrumDataPoint] = [];
        
        for i in 0..<c_erythema_action_spectrum.count
        {
            result.append(ErythemaActionSpectrumDataPoint(x:Double(290 + i), y: c_erythema_action_spectrum[i]))
        }
        
        return result
    }
    
    static func getVitaminDActionSpectrum() -> [VitaminDActionSpectrumDataPoint]
    {
        var result : [VitaminDActionSpectrumDataPoint] = [];
        
        for i in 0..<c_erythema_action_spectrum.count
        {
            result.append(VitaminDActionSpectrumDataPoint(x:Double(290 + i), y: c_vitamin_d_action_spectrum[i]))
        }
        
        return result
    }
    
    static func calculateSunAngleData(params: fastrt_params, fastrtresult: fastrt_result)
    -> [TimeAngleDataPoint]
    {
        let numSteps = 500;
        var angles = Array<Double>(repeating: 0, count: numSteps)
        var times = Array<Int32>(repeating: 0, count: numSteps)
        let seconds = -TimeZone.current.secondsFromGMT()
        let status = FastRT.get_day_sun_angle_data(&angles, &times, Int32(numSteps), Int32(params.julianday), Int32(seconds), params.lat, params.long, params.altitude)
        
        if (status != 0)
        {
            return []
        }
        var result : [TimeAngleDataPoint] = []
        
        let localStartTime = (params.seconds_since_midnight + TimeZone.current.secondsFromGMT() + 86400) % 86400
        let vitdEndTime = localStartTime + Int(fastrtresult.vitdtime)
        let erythemaEndTime = localStartTime + Int(fastrtresult.erythematime)
        
        var firstFlaggedPointPlaced = fastrtresult.vitdPercentDose < 1.0
        
        for i in 0..<numSteps
        {
            var vitd = false
            var erythema = false
            let t = (Int(times[i]) + TimeZone.current.secondsFromGMT() + 86400) % 86400
            if (t > localStartTime)
            {
                if (t <= vitdEndTime)
                {
                    vitd = true
                    firstFlaggedPointPlaced = true
                }
                if (t <= erythemaEndTime || fastrtresult.erythemaPercentDose < 1.0)
                {
                    erythema = true
                }
                if (!firstFlaggedPointPlaced)
                {
                    result.append(TimeAngleDataPoint(x:Double(localStartTime), y: 90 - angles[i], vitd: true, erythema: true))
                    firstFlaggedPointPlaced = true
                }
            }
            result.append(TimeAngleDataPoint(x:Double(t), y: 90 - angles[i], vitd: vitd, erythema: erythema))
        }
        
        return result
    }

    static func calculateExposureIntergralTimes(
        params : fastrt_params,
        targetVitD: Double? = nil,
        targetErythema: Double? = nil
    ) -> fastrt_result
    {
        // use static values if no targets provided
        let vitdTarget = targetVitD ?? c_vitamin_d_dose[params.fitzpatrick_skin_type]
        let erythemaTarget = targetErythema ?? c_erythema_dose[params.fitzpatrick_skin_type]
        
        let sliceSizeSeconds = 600.0
        var spectrumDoseData: [WaveLengthDataPoint] = init_spectrumDoseData()
        
        var vitaminDDoseTimeSeconds = 0.0
        var erythemaDoseTimeSeconds = 0.0
        
        var sliceParams = params
        var vitdSliceDose = 0.0
        var erythemaSliceDose = 0.0
        
        var sliceIndex = 0
        var vitdReached = false
        var erythemaReached = false
        while (!vitdReached || !erythemaReached)
        {
            let sliceDoseRates = fastrt_doserate(params: sliceParams, silent: true, spectrumDoseData: &spectrumDoseData)
            
            // stop summing if doserates are near-zero
            if (sliceDoseRates.0 < 1e-4 && sliceDoseRates.1 < 1e-4)
            {
                break;
            }
            if (!vitdReached && vitdSliceDose < vitdTarget)
            {
                vitdSliceDose += sliceDoseRates.0 * sliceSizeSeconds
                if (vitdSliceDose >= vitdTarget)
                {
                    vitaminDDoseTimeSeconds = Double(sliceIndex+1) * sliceSizeSeconds
                    let finalPartialSliceVitd = calculateDoseTime(dose: vitdSliceDose - vitdTarget, doserate: sliceDoseRates.0)
                    vitaminDDoseTimeSeconds -= finalPartialSliceVitd
                    vitdSliceDose = vitdTarget;
                    vitdReached = true
                }
            }
            if (!erythemaReached && erythemaSliceDose < erythemaTarget)
            {
                erythemaSliceDose += sliceDoseRates.1 * sliceSizeSeconds
                if (erythemaSliceDose >= erythemaTarget)
                {
                    erythemaDoseTimeSeconds = Double(sliceIndex+1) * sliceSizeSeconds
                    let finalPartialSliceErythema = calculateDoseTime(dose: erythemaSliceDose - erythemaTarget, doserate: sliceDoseRates.1)
                    erythemaDoseTimeSeconds -= finalPartialSliceErythema
                    erythemaSliceDose = erythemaTarget;
                    erythemaReached = true
                }
            }

            // increment slice index and reset slice doses
            sliceIndex += 1
            sliceParams.seconds_since_midnight = (sliceParams.seconds_since_midnight + Int(sliceSizeSeconds)) % 86400
            }

            let vitdPercentDose = vitdSliceDose/vitdTarget
            let erythemaPercentDose = erythemaSliceDose/erythemaTarget

            return fastrt_result(
            vitdtime: vitaminDDoseTimeSeconds,
            erythematime: erythemaDoseTimeSeconds,
            vitdPercentDose: vitdPercentDose,
            erythemaPercentDose: erythemaPercentDose,
            spectrumDoseData: spectrumDoseData)
        }
    
    static func calculateExposureTimes(
        params : fastrt_params)
    -> fastrt_result
    {
        var spectrumDoseData: [WaveLengthDataPoint] = init_spectrumDoseData()
        
        
        let doserates = fastrt_doserate(params: params, silent: false, spectrumDoseData: &spectrumDoseData)
        
        let vitaminDDoserate = doserates.0
        
        let erythemaDoserate = doserates.1
        
        let vitaminDDoseTimeSeconds =
        doserates.0 > 0 ? calculateDoseTime(dose: c_vitamin_d_dose[params.fitzpatrick_skin_type], doserate: vitaminDDoserate) : 0.0
        
        let erythemaDoseTimeSeconds =
        doserates.1 > 0 ? calculateDoseTime(dose: c_erythema_dose[params.fitzpatrick_skin_type], doserate: erythemaDoserate) : 0.0
        
        let vitdpercent = doserates.0 > 0 ? 1.0 : 0.0
        let erythemapercent = doserates.1 > 0 ? 1.0 : 0.0

        
        return fastrt_result(
            vitdtime: vitaminDDoseTimeSeconds,
            erythematime: erythemaDoseTimeSeconds,
            vitdPercentDose: vitdpercent,
            erythemaPercentDose: erythemapercent,
            spectrumDoseData: spectrumDoseData)
    }
    
    // Units are Joules / m^-2
    private static let c_erythema_dose = [ 200000.0, 250000.0, 300000.0, 450000.0, 600000.0, 1000000.0 ]
    private static let c_vitamin_d_dose = [ 21000.9, 27000.35, 32000.825, 49000.25, 65000.7, 109000.45 ]
    
    private static func init_spectrumDoseData()
    -> [WaveLengthDataPoint]
    {
        var result : [WaveLengthDataPoint] = []
        var i = Double(startWavelength)
        let d_endWaveLength = Double(endWavelength)
        
        while i <= d_endWaveLength
        {
            result.append(WaveLengthDataPoint(x: i, y: 0.0))
            i += stepWavelength
        }
        
        return result
    }
    
    private static func fastrt_doserate(
        params : fastrt_params,
        silent : Bool,
        spectrumDoseData: inout [WaveLengthDataPoint])
    -> (Double, Double)
    {
        let totalSteps = 1 + Int((Double(endWavelength) - Double(startWavelength)) / stepWavelength);
        var doserates = Array<Double>(repeating: 0, count: totalSteps)
        
//        FastRT.run_fastrt_test_inputs(&doserates)
        let status = FastRT.run_fastrt(
            &doserates,
            startWavelength,
            endWavelength,
            stepWavelength,
            Int32(params.julianday),
            params.lat,
            params.long,
            params.altitude,
            Int32(params.seconds_since_midnight),
            Int32(params.sky_condition_type),
            silent
        )
        
        if (status != 0)
        {
            return (0.0, 0.0)
        }
        
        var i1 = 0
        while i1 < doserates.count
        {
            if (doserates[i1].isNaN || doserates[i1] < 0.0)
            {
                doserates[i1] = 0.0;
            }
            spectrumDoseData[i1].y += doserates[i1]
            i1+=1
        }
        
        let vitaminDDoserate = params.exposed_skin * calculate_doserate(doserates: doserates, action_spectrum: c_vitamin_d_action_spectrum)
        
        let erythemaDoserate = calculate_doserate(doserates: doserates, action_spectrum: c_erythema_action_spectrum)
        
        return (vitaminDDoserate, erythemaDoserate)
    }
    
    private static func calculateDoseTime(dose: Double, doserate: Double) -> Double
    {
        // unit conversion to Watts/M^-2
        return dose / (doserate);
    }

    private static func calculate_doserate(doserates: Array<Double> , action_spectrum: [Double] ) -> Double
    {
        var dose = 0.0
        
        for i in 0..<doserates.count
        {
            if (!doserates[i].isNaN && doserates[i].isFinite)
            {
                dose += doserates[i] *  action_spectrum[i];
            }
        }
        return dose * 0.8 // magic number since there's this constant approximate differential between my implementation and the reference tool across all doses
    }
    
    
    static let c_vitamin_d_action_spectrum = [ 8.780e-01, 9.030e-01, 9.280e-01, 9.520e-01, 9.760e-01, 9.830e-01, 9.900e-01, 9.960e-01, 1.000e+00, 9.770e-01, 9.510e-01, 9.170e-01, 8.780e-01, 7.710e-01, 7.010e-01, 6.340e-01, 5.660e-01, 4.880e-01, 3.950e-01, 3.060e-01, 2.200e-01, 1.560e-01, 1.190e-01, 8.300e-02, 4.900e-02, 3.400e-02, 2.000e-02, 1.410e-02, 9.760e-03, 6.520e-03, 4.360e-03, 2.920e-03, 1.950e-03, 1.310e-03, 8.730e-04, 5.840e-04, 3.900e-04, 2.610e-04, 1.750e-04, 1.170e-04, 7.800e-05, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00 ]
    
    static let c_erythema_action_spectrum = [ 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 8.054e-01, 6.486e-01, 5.224e-01, 4.207e-01, 3.388e-01, 2.729e-01, 2.198e-01, 1.770e-01, 1.426e-01, 1.148e-01, 9.247e-02, 7.447e-02, 5.998e-02, 4.831e-02, 3.891e-02, 3.133e-02, 2.524e-02, 2.032e-02, 1.637e-02, 1.318e-02, 1.062e-02, 8.551e-03, 6.887e-03, 5.546e-03, 4.467e-03, 3.598e-03, 2.897e-03, 2.334e-03, 1.879e-03, 1.514e-03, 1.412e-03, 1.365e-03, 1.318e-03, 1.273e-03, 1.230e-03, 1.189e-03, 1.148e-03, 1.109e-03, 1.071e-03, 1.035e-03, 1.000e-03, 9.660e-04, 9.333e-04, 9.016e-04, 8.710e-04, 8.414e-04, 8.128e-04, 7.852e-04, 7.586e-04, 7.328e-04, 7.080e-04, 6.839e-04, 6.607e-04, 6.383e-04, 6.166e-04, 5.957e-04, 5.754e-04, 5.559e-04, 5.370e-04, 5.188e-04, 5.012e-04, 4.842e-04, 4.677e-04, 4.519e-04, 4.365e-04, 4.217e-04, 4.074e-04, 3.935e-04, 3.802e-04, 3.673e-04, 3.548e-04, 3.428e-04, 3.311e-04, 3.199e-04, 3.090e-04, 2.985e-04, 2.884e-04, 2.786e-04, 2.692e-04, 2.600e-04, 2.512e-04, 2.427e-04, 2.344e-04, 2.265e-04, 2.188e-04, 2.113e-04, 2.042e-04, 1.972e-04, 1.905e-04, 1.841e-04, 1.778e-04, 1.718e-04, 1.660e-04, 1.603e-04, 1.549e-04, 1.496e-04, 1.445e-04, 1.396e-04, 1.349e-04, 1.303e-04, 1.259e-04, 1.216e-04 ]
}
