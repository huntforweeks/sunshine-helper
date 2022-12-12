#/* Producing look-up table data for the fastrt software                 */
#/*                                                                      */
#/* Author: Ola Engelsen,                                                */
#/*        Norwegian Institute for Air Research (NILU)                   */
#/*        N-9296 Tromso,                                                */
#/*        Norway                                                        */
#/*        Email: ola.engelsen@nilu.no                                   */
#/*                                                                      */
#/*----------------------------------------------------------------------*/
#/* Copyright (C) 2001 Ola Engelsen                                      */
#/*                                                                      */
#/* This program is free software; you can redistribute it and/or modify */
#/* it under the terms of the GNU General Public License as published by */
#/* the Free Software Foundation; either version 1, or (at your option)  */
#/* any later version.                                                   */
#/*                                                                      */
#/* This program is distributed in the hope that it will be useful,      */
#/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
#/* MERCHANTABILITY of FITNESS FOR A PARTICULAR PURPOSE. See the         */
#/* GNU General Public License for more details.                         */
#/*                                                                      */
#/* To obtain a copy of the GNU General Public License write to the      */
#/* Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,   */
#/* USA.                                                                 */
#/*----------------------------------------------------------------------*/
#/************************************************************************/

use Getopt::Std;

sub set_altitude_write_atmmod($spline, $new_atmmod_file, $atmmod_file, $zout)
{

    $tmp1="dummyfile1";
    $tmp2="dummyfile2";
    $alt_file="altfile";
    $new_atmmod_file=$atmmod_file."_surface${zout}km";

    # Reverse file for spline_interpolation and
    # Generate altitude file for spline
    open(INP,$atmmod_file);
    @lines = <INP>;
    close(INP);
    open(OUT,">".$tmp1);
    open(ALT,">".$alt_file);
    print ALT "$zout\n";
    for ($i=$#lines;$i>=0;$i--) {
	$line = @lines[$i];
	$line =~ s/^\s*//;
	($alt, $remainder) = split(/\s+/, $line);
	if ($alt =~ /^#/) {
	    @header[$i]=@lines[$i];
	}
	else {
	    if ( $alt > $zout) {
		print ALT "$alt\n";
	    }
	    print OUT @lines[$i];
	}			
    }	
    close(ALT);
    close(OUT);

    # Interpolate to new grid
    system("$spline -x $alt_file $tmp1 > $tmp2");

    # Reverse file for uvspec
    open(INP,$tmp2);
    @lines = <INP>;
    close(INP);
    open(OUT,">".$new_atmmod_file);

    print OUT @header[0];
    print OUT @header[1];  
    for ($i=$#lines;$i>=0;$i--) {
	($gabba, $altitude, $air_pressure, $temperature, $air, $o3, $o2, $h2o, $co2, $no2) = 
	    split(/\s+/,@lines[$i]);
	printf OUT "%10.3f %11.5f %8.3f %13.6e %13.6e %13.6e %13.6e %13.6e %13.6e\n",
	$altitude, $air_pressure, $temperature, $air, $o3, $o2, $h2o, $co2, $no2
    }				
    close(OUT);
    system("rm $tmp1 $tmp2 $alt_file");
}

sub print_usage()
{
  print "\nComputes look-up table (LUT) for the\n";
  print "fast and easy radiative transfer model (fastrt)\n";
  print "Computes exact UV transmittances (incl. surface effects) using\n";
  print "the highly accurate libRadtran radiative transfer package.\n";
  print "\n";
  print "Usage: perl produce_table.pl [-hstuopqwxyfklrab]\n";
  print " -h prints this message.\n";
  print " -s start solar zenith angle (default 0 degrees)\n";
  print " -t end solar zenith angle (default 87 degrees)\n";
  print " -u step solar zenith angle (default 3 degrees)\n";
  print " -o start ozone column (default 100 DU)\n";
  print " -p end ozone column (default 600 DU)\n";
  print " -q step ozone column (default 20 DU) \n";
#  print " -w start_wavelength (default 290 nm)\n";
#  print " -x end_wavelength (default 405 nm)\n";
#  print " -y step_wavelength (default 0.5 nm)\n";
  print " -f wavelength filename (w,x,y options ignored) (default lambdafile\n";
  print " -k lower_altitude (default 0 km)\n";
  print " -l top_altitude (default 6 km)\n";
  print " -a aerosol angstrom coefficient alpha (default 1.3)\n";
  print "\n";
  print "Examples:\n";
  print "perl ./produce_table.pl -s 70 -t 75 -u 5 -o 350 -p 360 -q 10 -w 290 -x 320 -y 0.5 -a 1.3 -z 0.732\n";
  print "\n";
  print "Output:\n";
  print "* files in './transmittances/. directory named szaxxxozoneyyyaltzzz where xxx, yyy and zzz represents\n";
  print "the solar zenith angle (deg), ozone content (DU) and altitude (km) respectively\n";
  print "The files contain a single column representing transmittance\n";
  print "* In addition the program generates the file rawlambdafile containing the corresponding wavelengths (nm)\n";
  print "* files in './atmospheric_reflectivities/. named altzzz\n";
  print "\n";
}

sub write_radtran_inp($radtranpath, $radtran_inp, $atmmod_file, $ozone_column, 
		      $albedo, $sza, $zout, $alpha, $beta, $visibility, $solar_file,
		      $lambda_start, $lambda_end, $reverse)
{
    open(OUT, ">$radtran_inp")  || die "\nWrite_radtran_inp: couldn't open $radtran_inp\n";
    $day_formatted=sprintf "%d", $day;
    print OUT "atmosphere_file $atmmod_file\n";
    $ozone_column_formatted=sprintf "%10.4f", $ozone_column;
    print OUT "ozone_column $ozone_column_formatted\n";
    $albedo_formatted=sprintf "%10.4f", $albedo;
    print OUT "albedo $albedo_formatted\n";
    $sza_formatted=sprintf "%10.4f", $sza;
    print OUT "sza $sza_formatted\n";
    $alpha_formatted=sprintf "%10.4f", $alpha;
    $beta_formatted=sprintf "%10.4f", $beta;
    print OUT "angstrom $alpha_formatted $beta_formatted\n";
    print OUT "solar_file $solar_file\n";
    $visibility_formatted=sprintf "%10.4f", $visibility;
    print OUT "aerosol_visibility $visibility_formatted\n";
    print OUT "o3_crs Molina\n";
    $lambda_start_formatted=sprintf "%10.4f", $lambda_start;
    $lambda_end_formatted=sprintf "%10.4f", $lambda_end;
    print OUT "wvn $lambda_start_formatted $lambda_end_formatted\n";
    print OUT "aerosol_season 1\n";
    print OUT "aerosol_haze 1\n";
    print OUT "aerosol_vulcan 1\n";
    print OUT "data_files_path ".$radtranpath."../data/\n";
    print OUT "deltam  on\n";
    print OUT "nstr  12\n";
    if ($reverse == 1) {
	print OUT "fisot\n";                    # diffuse illumination instead of direct
	print OUT "reverse\n";                  # turn the atmosphere upside down
	print OUT "zout 120.0\n";               # output at top which is now bottom
	print OUT "rte_solver disort\n";        # reverse option only work for disort rte solver
    }
    else {
	$zout_formatted=sprintf "%10.4f", $zout;
	print OUT "zout $zout_formatted\n";
	print OUT "rte_solver sdisort\n";
    }
    close(OUT);
}

$opt_h=0;
$opt_s=0;
$opt_t=87;
$opt_u=3;
$opt_o=100;
$opt_p=600;
$opt_q=20;
$opt_w=290;
$opt_x=405;
$opt_y=0.5; 
#$opt_f="lambdafile";
$opt_k=0.0;
$opt_l=6.0;
$opt_a=1.3;

$ok = getopts('hs:t:u:o:p:q:w:x:y:f:k:l:a:');
if (!$ok) {
    print "invalid input\n";
    &print_usage();
    exit(0);
}

if ($opt_h) {
    &print_usage();
    exit(0);
}


$radtranpath="../tools/";
$atmmod_file = "../data/atmmod/afglus.dat";
$albedo = $opt_r;
$zout = $opt_z;
$alpha=$opt_a;
$beta = 0.;
$visibility = 100.;
$lambda_start=$opt_w;
$lambda_end=$opt_x;
$radtran_inp = "tmp.inp";
$radtran_out = "tmp.out";
$radtran_out_conv= "${radtran_out}_conv";

$radtran_run=$radtranpath."uvspec";
$spline=$radtranpath."spline";
$solar_file="unit_solar_file";
$new_atmmod_file="temp";


system("mkdir data_tmp");
system("mkdir transmittances");
system("mkdir atmospheric_reflectivities");

if ($opt_f){
    $lambda_start=290;
    $lambda_end=405;
    system("cp $opt_f transmittances/rawlambdafile");
} 
else { 
    open(LAM,">transmittances/rawlambdafile") or 
	die "Cannot open rawlambdafile\n";
    for ($lambda=$lambda_start;$lambda<=$lambda_end;$lambda+=$opt_y) {
	printf LAM "%6.2f\n", $lambda;
    }
    close(LAM);
}

open(SOL,">$solar_file") or 
    die "Cannot create solar file\n";
printf SOL "#unit extraterrestrial fluxes to obtain atmospheric transmission\n";
for ($lambda=200.; $lambda<420.; $lambda+=0.05) {
    printf SOL "%6.2f %8.5f\n", $lambda, 1.;
}
close(SOL);


$reverse=0; # zero flag for computing transmittances
$zstep = ($opt_l-$opt_k)/2.;
if ($zstep == 0.) { $zstep = 200.}
for ($zout=$opt_k; $zout<=$opt_l; $zout+=$zstep) {
    if ($zout != 0.) {
	&set_altitude_write_atmmod($spline, $new_atmmod_file, $atmmod_file, $zout);
    }
    else {
	system("cp $atmmod_file $new_atmmod_file");
    } 

    for ($xsza=$opt_s; $xsza<=$opt_t; $xsza+=$opt_u) {
	if ($xsza == 0.) { # avoid DISORT singularity
	    $sza=0.15;
	}
	else {
	    $sza=$xsza;
	}
	for ($ozone_column=$opt_o; $ozone_column<=$opt_p; $ozone_column+=$opt_q) {
	    print "$xsza $ozone_column $zout\n";
	    &write_radtran_inp($radtranpath, $radtran_inp, $new_atmmod_file, 
			       $ozone_column, $albedo, $sza, $zout, $beta, $visibility, 
			       $solar_file, $lambda_start, $lambda_end, $reverse);
	    system("$radtran_run < $radtran_inp > $radtran_out");
	    if ($opt_f){
		$cmdinp= "-x $opt_f $radtran_out > data_tmp/sza${xsza}ozone${ozone_column}alt${zout}";
	    }
	    else {
		$cmdinp= "-b $lambda_start -s $opt_y $radtran_out > data_tmp/sza${xsza}ozone${ozone_column}alt${zout}";
	    }
	    system("$spline $cmdinp");
	    open(IN, "data_tmp/sza${xsza}ozone${ozone_column}alt${zout}") or 
		die "Cannot open data_tmp/sza${xsza}ozone${ozone_column}alt${zout}\n";
	    open(OUT, ">transmittances/sza${xsza}ozone${ozone_column}alt${zout}") or 
		die "Cannot open transmittances/xsza${sza}ozone${ozone_column}alt${zout}\n";
	    while ($line= <IN>) {
		($dummy,$lambda, $fdir, $fdif, $dummy) = split /\s+/, $line, 5;
		printf OUT "%12.6e\n", $fdir+$fdif;
	    }
	    close(IN);
	    close(OUT);
	    system("rm $radtran_inp $radtran_out");
	    system("rm data_tmp/*");
	}
    }
}

# generate atmospherical atmospheric albedo files
$reverse=1; # set flag for computing atmospheric spherical albedo from below
my $pi = 4*atan2(1,1);
for ($zout=$opt_k; $zout<=$opt_l; $zout+=$zstep) {
    if ($zout != 0.) {
	&set_altitude_write_atmmod($spline, $new_atmmod_file, $atmmod_file, $zout);
    }
    else {
	system("cp $atmmod_file $new_atmmod_file");
    } 
    print "$zout\n";
    &write_radtran_inp($radtranpath, $radtran_inp, $new_atmmod_file, 
		       $ozone_column, $albedo, $sza, $zout, $beta, $visibility, 
		       $solar_file, $lambda_start, $lambda_end, $reverse);
    system("$radtran_run < $radtran_inp > $radtran_out");
    if ($opt_f){
	$cmdinp= "-x $opt_f $radtran_out > data_tmp/alt${zout}";
    }
    else {
	$cmdinp= "-b $lambda_start -s $opt_y $radtran_out > data_tmp/alt${zout}";
    }
    system("$spline $cmdinp");
    open(IN, "data_tmp/alt${zout}") or 
	die "Cannot open data_tmp/alt${zout}\n";
    open(OUT, ">atmospheric_reflectivities/alt${zout}") or 
	die "Cannot open atmospheric_reflectivities/alt${zout}\n";
    while (my $line= <IN>) {
	$line =~ s/^[ ]+//;
	my ($wvn1,$dir1,$dn1,$up1) = split(/ +/,$line);
	$atmref = $up1/$pi; # Divide by pi to get reflectivity
	printf OUT "%13.6e\n", $atmref;
    }
    close(IN);
    close(OUT);
    system("rm $radtran_inp $radtran_out");
    system("rm data_tmp/*");
}

system("rm -rf data_tmp");
system("rm $new_atmmod_file");
system("rm unit_solar_file");
