import Foundation
import FastRT

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

class spectrum_helpers {
    // Units are Joules / m^-2
    private static let c_erythema_dose = [ 200000.0, 250000.0, 300000.0, 450000.0, 600000.0, 1000000.0 ]
    private static let c_vitamin_d_dose = [ 21000.9, 27000.35, 32000.825, 49000.25, 65000.7, 109000.45 ]
    
    private static func fastrt_doserate(
        params : fastrt_params,
        silent : Bool)
    -> (Double, Double)
    {
        let startWavelength = Int32(290);
        let endWavelength = Int32(400);
        let stepWavelength = 1.0;
        
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
        
        let vitaminDDoserate = params.exposed_skin * calculate_doserate(doserates: doserates, action_spectrum: c_vitamin_d_action_spectrum)
        
        let erythemaDoserate = calculate_doserate(doserates: doserates, action_spectrum: c_erythema_action_spectrum)
        
        return (vitaminDDoserate, erythemaDoserate)
    }
    
    static func calculateExposureIntergralTimes(
        params : fastrt_params)
    -> (vitdtime: Double, erythematime:  Double, vitdPercentDose : Double, erythemaPercentDose : Double)
    {
        let sliceSizeSeconds = 600.0
        let maxIterations = Int(86400 / sliceSizeSeconds)

        
        var vitaminDDoseTimeSeconds = 0.0
        var erythemaDoseTimeSeconds = 0.0
        
        let vitdTarget = c_vitamin_d_dose[params.fitzpatrick_skin_type]
        let erythemaTarget = c_erythema_dose[params.fitzpatrick_skin_type]
        var vitdDose = 0.0
        var erythemaDose = 0.0
        
        var sliceParams = params
        
        for sliceIndex in 0..<maxIterations
        {
            let sliceDoseRates = fastrt_doserate(params: sliceParams, silent: true)
            // stop summing if doserates are near-zero
            if (sliceDoseRates.0 < 1e-4 && sliceDoseRates.1 < 1e-4)
            {
                break;
            }
            if (vitdDose < vitdTarget)
            {
                vitdDose += sliceDoseRates.0 * sliceSizeSeconds
                if (vitdDose >= vitdTarget)
                {
                    vitaminDDoseTimeSeconds = Double(sliceIndex+1) * sliceSizeSeconds
                    let finalPartialSliceVitd = calculateDoseTime(dose: vitdDose - vitdTarget, doserate: sliceDoseRates.0)
                    vitaminDDoseTimeSeconds -= finalPartialSliceVitd
                    vitdDose = vitdTarget;
                }
            }
            if (erythemaDose < erythemaTarget)
            {
                erythemaDose += sliceDoseRates.1 * sliceSizeSeconds
                if (erythemaDose >= erythemaTarget)
                {
                    erythemaDoseTimeSeconds = Double(sliceIndex+1) * sliceSizeSeconds
                    let finalPartialSliceErythema = calculateDoseTime(dose: erythemaDose - erythemaTarget, doserate: sliceDoseRates.1)
                    erythemaDoseTimeSeconds -= finalPartialSliceErythema
                    erythemaDose = erythemaTarget;
                }
            }
            // found both target times
            if (erythemaDoseTimeSeconds > 0.0 && vitaminDDoseTimeSeconds > 0.0)
            {
                break;
            }
            sliceParams.seconds_since_midnight =
            (sliceParams.seconds_since_midnight + Int(sliceSizeSeconds)) % 86400
        }

        return (vitaminDDoseTimeSeconds,erythemaDoseTimeSeconds,vitdDose/vitdTarget,erythemaDose/erythemaTarget)
    }
    
    static func calculateExposureTimes(
        params : fastrt_params)
    -> (vitdtime: Double, erythematime:  Double)
    {
        let doserates = fastrt_doserate(params: params, silent: false)
        
        let vitaminDDoserate = doserates.0
        
        let erythemaDoserate = doserates.1
        
        
        let vitaminDDoseTimeSeconds =
        doserates.0 > 0 ? calculateDoseTime(dose: c_vitamin_d_dose[params.fitzpatrick_skin_type], doserate: vitaminDDoserate) : 0.0
        
        let erythemaDoseTimeSeconds =
        doserates.1 > 0 ? calculateDoseTime(dose: c_erythema_dose[params.fitzpatrick_skin_type], doserate: erythemaDoserate) : 0.0
        
        return (vitaminDDoseTimeSeconds,erythemaDoseTimeSeconds)
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
            if (!doserates[i].isNaN)
            {
                dose += doserates[i] *  action_spectrum[i];
            }
        }
        return dose
    }
    
    
    private static let c_vitamin_d_action_spectrum = [ 8.780e-01, 9.030e-01, 9.280e-01, 9.520e-01, 9.760e-01, 9.830e-01, 9.900e-01, 9.960e-01, 1.000e+00, 9.770e-01, 9.510e-01, 9.170e-01, 8.780e-01, 7.710e-01, 7.010e-01, 6.340e-01, 5.660e-01, 4.880e-01, 3.950e-01, 3.060e-01, 2.200e-01, 1.560e-01, 1.190e-01, 8.300e-02, 4.900e-02, 3.400e-02, 2.000e-02, 1.410e-02, 9.760e-03, 6.520e-03, 4.360e-03, 2.920e-03, 1.950e-03, 1.310e-03, 8.730e-04, 5.840e-04, 3.900e-04, 2.610e-04, 1.750e-04, 1.170e-04, 7.800e-05, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00 ]
    
    private static let c_erythema_action_spectrum = [ 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 1.000e+00, 8.054e-01, 6.486e-01, 5.224e-01, 4.207e-01, 3.388e-01, 2.729e-01, 2.198e-01, 1.770e-01, 1.426e-01, 1.148e-01, 9.247e-02, 7.447e-02, 5.998e-02, 4.831e-02, 3.891e-02, 3.133e-02, 2.524e-02, 2.032e-02, 1.637e-02, 1.318e-02, 1.062e-02, 8.551e-03, 6.887e-03, 5.546e-03, 4.467e-03, 3.598e-03, 2.897e-03, 2.334e-03, 1.879e-03, 1.514e-03, 1.412e-03, 1.365e-03, 1.318e-03, 1.273e-03, 1.230e-03, 1.189e-03, 1.148e-03, 1.109e-03, 1.071e-03, 1.035e-03, 1.000e-03, 9.660e-04, 9.333e-04, 9.016e-04, 8.710e-04, 8.414e-04, 8.128e-04, 7.852e-04, 7.586e-04, 7.328e-04, 7.080e-04, 6.839e-04, 6.607e-04, 6.383e-04, 6.166e-04, 5.957e-04, 5.754e-04, 5.559e-04, 5.370e-04, 5.188e-04, 5.012e-04, 4.842e-04, 4.677e-04, 4.519e-04, 4.365e-04, 4.217e-04, 4.074e-04, 3.935e-04, 3.802e-04, 3.673e-04, 3.548e-04, 3.428e-04, 3.311e-04, 3.199e-04, 3.090e-04, 2.985e-04, 2.884e-04, 2.786e-04, 2.692e-04, 2.600e-04, 2.512e-04, 2.427e-04, 2.344e-04, 2.265e-04, 2.188e-04, 2.113e-04, 2.042e-04, 1.972e-04, 1.905e-04, 1.841e-04, 1.778e-04, 1.718e-04, 1.660e-04, 1.603e-04, 1.549e-04, 1.496e-04, 1.445e-04, 1.396e-04, 1.349e-04, 1.303e-04, 1.259e-04, 1.216e-04 ]
    
}
