import Foundation

class FastRT_Lib {
    static let c_delta_sza = 3;
    static let c_delta_03 = 20;
    static let c_delta_alt = 3;
    static let c_fwhm_default = 0.6;
    static let c_solar_flux_resolution = 0.05;
    static let c_albedo_resolution = 0.05;
    static let c_cloud_thickness = 5;
    
    static let DOUBLE_RELATIVE_ERROR = 1e-10;
    static let pi = 3.14159265358979323846264338327;
    
    
    
    // calculation input vars
    var month: Int
    var mday: Int
    var city: Int // unused?
    var location_specification: Int
    var latitude: double
    var longitude: double
    var sza_override: Bool
    var sza_angle: Int
    var skin_index: Int
    var exposure_timing: Int
    var start_time: Double
    var uvi_flag: Int
    var body_exposure: Int
    var dietary_equivalent: Int
    var sky_condition: Int
    var aerosol_specification: Int
    var visibility: Int
    var angstrom_beta: Double
    var cloud_fraction: Int
    var wc_column1: Int
    var wc_column2: Int
    var wc_column3: Int
    var uvi: double
    var ozone_column: Int
    var altitude: Int
    var surface: Int
    var albedo: double
    var calculation_type: Int
    
    var sza_flag=0, ozone_flag=0, alt_flag=0,
        albedo_flag=0, albedo_file_flag=0, albedo_type_flag=0,
        beta_flag=0, broken_cloud_flag=0, cloudH2O_flag=0,
        start_lambda_flag=0, end_lambda_flag=0, day_flag=0,
        step_lambda_flag=0, x_flag=0, fwhm_flag=0, sr_flag=0;
    
    func perform_exposure_calculations() -> Bool
    {
        var global_irradiance, tau550: Double
        
        var alt=0.0, start_lambda=0.0, end_lambda=0.0, step_lambda=0.0;
        
        var sza=0.0, o3=0.0, beta=0.0,
            cloudOD = 0.0,  cloudH2O = 0.0, cloudH2O_low = 0.0, cloudH2O_high = 0.0,
            visibility, fwhm=0.0, day_corr= 0.0, angle= 0.0, day=0.0,
            alb=0, AtmAlbFactor=1.0;
        
        var cloud_H20_array = [0.000, 0.005, 0.014, 0.029, 0.057, 0.109, 0.217, 0.460, 1.000];
        
        // FLAGS
        if (sza_flag)
        {
            sza = sza_angle;
        }
        if (beta_flag)
        {
            tau550 = (3.912/visibility-0.01162)*(0.02472*(visibility-5.0)+1.132);
            beta = tau550 * pow(0.55,1.3);
        }
        if (cloudH2O_flag)
        {
            print("cloudH20 flag not done yet");
//            cloudOD = cloud_fraction;
//            cloudH2O= cloudOD/1083.0;
//            var i = 0;
//            while cloud_H20_array[i] < cloudH2O {
//                i++;
//            }
//            if (cloud_H20_array[i] == cloudH2O) {
//                x_cloudH20[0]= cloudH2O;
//                subscr_cloudH20_max= 0;
//                cloudH20_high= cloudH20;
//                cloudH20_low= cloud
//            }
        }
        if (alt_flag)
        {
            alt= altitude;
            if (alt < 0)
                print("warning: surface altitude less than 0km");
            if (alt > 6.0)
                print("warning: surface altitude greater than 6km");
        }
        if (albedo_flag)
        {
            print("albedo flag not implemented");
        }
        if (albedo_type_flag)
        {
            if (surface < 0)
                print("error, surface # less than 0");
            if (surface > 17)
                print("error, surface # greater than 17");
        }
        if (albedo_file_flag)
        {
            print("albedo file flag not implemented");
        }
        if (fwhm_flag)
        {
            print("fwhm flag not implemented");
        }
        if (sr_flag)
        {
            print("sr flag not implemented");
        }
        if (start_lambda_flag)
        {
            print("start lambda flag not implemented");
        }
        if (end_lambda_flag)
        {
            print("end lambda flag not implemented");
        }
        // END FLAGS
        
        if (!sza_flag || !ozone_flag)
        {
            print("Solar zenith angle (sza) and ozone must be specified");
        }
        
        if (day_flag)
        {
            /* correct for deviations from average sun-earth distance
           From J. Lenoble, "Atmospheric Radiative Transfer", 1993, A. Deepak Publishing */
            let angle = 2.0 * pi * ((day-1) / 365.0);
            day_corr = 1.000110 + 0.034221 * cos(angle) + 0.001280 * sin(angle)
              + 0.000719 * cos(2*angle) + 0.000077 * sin(2*angle);
        }
        else
        {
            day_corr= 1.0
        }
//        if (albedo_type_flag) {
//          for (i = 0; i < n_lambda; i++) {
//            index = (int) ((lambda[i]-290.)/ALBEDO_RESOLUTION + 0.5);
//            albedo[i] = surface_albedo[surfaceno][index];
//          }
//        }
    }
    
    
    func double_equal(a: double, b: double) -> Bool
    {
        let diff = fabs(a-b);
        if (diff == 0.0)
            return true;
        if (a == 0 | b == 0)
            return false;
        a = fabs(a);
        b = fabs(b);
        
        if (a > b)
            swap(a, b);
        
        if (diff/a < DOUBLE_RELATIVE_ERROR)
            return true;
        return false;
        
    }
    
    func make_slitfunction(
        fwhm: double,
        sr_lambda_array: inout [double],
        sr_array: inout [double],
        sr_nlambda: inout int)
    {
        let resolution= c_solar_flux_resolution;
        let lambda_range = 2 * fwhm;
        let n_points = floor(lambda_range/resolution+0.5) + 1;
        let sr_step = 1.0/(n_points/2);
        
        var j=-1;
        for i in 0..<(n_points/2) {
            j++;
            sr_lambda_array[j] = (-lambda_range)/2.0 + (i * resolution);
            sr_array[j] = i * sr_step;
        }
        for i in 0...(n_points/2) {
            j++;
            sr_lambda_array[j] = i * resolution;
            sr_array[j] = (n_points/2 - i) * sr_step;
        }
        sr_nlambda = n_points;
    }
    
    func check_spectral_response_function(
        sr_lambda: [double],
        sr: [double],
        sr_nlambda: int) -> Bool
    {
        var conv_delta = 0.0;
        /* check for equidistant relative wavelengths in slitfunction */
        if (sr_nlambda >= 3)
        {
            conv_delta = sr_lambda[1] - sr_lambda[0];
            
            for i in 2..<sr_nlambda {
                if (! double_equal(a: sr_lambda[i]-sr_lambda[i-1], b: conv_delta)){
                    print(" ... wavelengths in slitfunction are not equidistant!");
                    print(String(format: "%@%f%@", "... FWHM must be multiple of ", c_solar_flux_resolution, " nm"));
                    return false;
                }
            }
        }
        else {
            print("... ERROR: less than 3 slitfunction elements.");
            print(String(format: "%@%f%@",
                         "For a Kronecker delta slitfunction generate a triangular slitfunction of ", c_solar_flux_resolution, " nm FWHM"));
        }
        
        if (sr_lambda[0] > 250) {
            /* absolute wavelengths provided, convert to relative wavelengths */
            /*find central wavelength */
            var sum = 0.0;
            var sum0 = 0.0;
            for i in 0..<sr_nlambda {
                sum += sr[i] * sr_lambda[i];
                sum0 += sr[i];
            }
            
            let central_lambda = c_solar_flux_resolution * floor(sum/sum0/c_solar_flux_resolution+0.5);
            /* convert to relative wavelengths */
            for i in 0..<sr_nlambda {
                sr_lambda[i] = sr_lambda[i]-central_lambda;
            }
        }
        
        return true;
    }
    
    func do_spectra() { return; }
    
    func newton_co(np: int, x: [double], y: [double]) -> [double]
    {
        var a: Array<Double> = Array();
        
        for i in 0..<np {
            a[i] = y[i];
        }
        
        for j in 1..<np {
            for i in np...j+1 {
                a[i-1] = (a[i-1] - a[i-2] / (x[i-1] - x[i-j-1]));
            }
        }
        
        return a;
    }
    
    func newton_eval(n: int, x: [double], y: [double], t: [double]) -> double
    {
        var ev = a[n-1];
        for i in n-2...0 {
            ev = ev * (t - x[i]) + a[i];
        }
        return ev;
    }
    

}
