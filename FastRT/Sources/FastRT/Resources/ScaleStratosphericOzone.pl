use Getopt::Std;

sub o3column 
#
#BOD
#DESCRIPTION
# Computes ozone_column.
#USAGE
#INPUT
##dens
#     real array(#)
#     Ozone density in number of molecules per cubic centimeter.
##nlyr
#     integer
#     Number of atmospheric layers.
#*zstart
#     real array(*)
#     The altitude above which to compute ozone column.
##zd
#     real array(*)
#     The altitude of each level.
#OUTPUT
#*o3column 
#     real
#     Ozone column.
#ERROR_MESSAGES
#ROUTINES_CALLED
#FILES
#REFERENCES
#EOD
#
{
    local(@input) = @_;
    $input = @input;
    if ( $input != 4 ) {die "o3column: wrong number of input: $input\n";}
    local($dens, $nlyr, $zstart, $zd) = @_;
    $colo3 = 0.0;
    for ($lc=0;$lc<$nlyr;$lc++){
	if ($zd[$lc]>=$zstart){ 
#    print "$zd[$lc] $dens[$lc]\n";
	    $deltaz = ( $zd[$lc]-$zd[$lc+1] ) * 1.E+5; # Convert from km to cm
	    $colo3 = $colo3 + 0.5 * ($dens[$lc]+$dens[$lc+1]) * $deltaz;
	}
    }
    $o3column = $colo3 / 2.687E+16; # Convert to dobson units
    $o3column;
}
    

sub scaleozone 
#
#BOD
#DESCRIPTION
# Scales the ozone profile dens to ozone_column.
#USAGE
#*FORTRAN
#     CALL scale_ozone(dens, nlyr, ozone_column, zd) 
#*PERL
#INPUT
#*dens
#     real array(*)
#     Ozone density in number of molecules per cubic centimeter.
#*nlyr
#     integer
#     Number of atmospheric layers.
#*ozone_column
#     real
#     The ozone column in DU (Doboson units). dens will be scaled 
#     to ozone_column.
#*zstart
#     real array(*)
#     The altitude above which to scale the ozone density.
#*zd
#     real array(*)
#     The altitude of each level.
#OUTPUT
#*dens
#     real array(*)
#     Ozone density in number of molecules per cubic centimeter
#     scaled to ozone_column.
#ERROR_MESSAGES
#ROUTINES_CALLED
#FILES
#REFERENCES
#EOD
#
{
    local(@input) = @_;
    $input = @input;
    if ( $input != 6 ) {die "scaleozone: wrong number of input: $input\n";}
    local($dens, $nlyr, $ozone_column, $zstart, $zd, $fact) = @_;
#
# Calculate the ozone column in dobson
#
    $colo3_du = o3column($dens, $nlyr, 0., $zd );
    $colo3_du_strat = o3column($dens, $nlyr, $zstart, $zd );
    $colo3_du_trop=$colo3_du-$colo3_du_strat;
# 
# Scale profile to ozone_column
#  
    $fact  = ($ozone_column-$colo3_du_trop)/$colo3_du_strat;
#    print "$ozone_column $colo3_du $fact\n";
    
    for ($lc=0;$lc<=$nlyr;$lc++) {
	if ($zd[$lc]>=$zstart){ 
	    $dens[$lc] = $dens[$lc] * $fact;
        }
    }
    $colo3_du = o3column($dens, $nlyr, 0., $zd );
#    print "$ozone_column $colo3_du\n";
    @dens;
}

sub ScaleStratosphericOzone
{
    local(@input) = @_;
    $input = @input;
#    print "inpt @input\n";
    if ( $input != 4 ) {die "scaleozone: wrong number of input\n";}
    local($atmfile, $ozone_column, $zstart, $NewAtmFile) = @_;
    
    open(ATM, $atmfile);
    $nlyr=-1;
  LINE:
    while($line=<ATM>)  {
	$_ = $line;                   # Make m match $line
	if (m/^[\#]/) {  next LINE; } # Skip comment lines
	$nlyr++;	
	$line =~ s/^[ ]*//;
	chop $line;
	($z, $p, $T, $air, $o3, $o2, $h2o, $co2, $no2) = split(/[ \t]+/, $line);
	$zd[$nlyr]=$z;
	$ps[$nlyr] = $p;
	$Ts[$nlyr] = $T;
	$airs[$nlyr]= $air;
	$dens[$nlyr] = $o3;
	$o2s[$nlyr] = $o2;
	$h2os[$nlyr] = $h2o;
	$co2s[$nlyr] = $co2;
	$no2s[$nlyr]= $no2;
    }
    close(ATM);
    @newdens=scaleozone($dens, $nlyr, $ozone_column, $zstart, $zd, $fact);
    open(OUT, ">$NewAtmFile");
print OUT "# scaled AFGL atmospheric constituent profile. U.S. standard atmosphere 1976. ( AFGL-TR-86-0110) \n";      
print OUT "#     z(km)      p(mb)        T(K)    air(cm-3)    o3(cm-3)     o2(cm-3)    h2o(cm-3)    co2(cm-3)     no2(cm-3) \n";
    for($i=0;$i<=$nlyr;$i++){
	printf OUT "%11.3f %12.5f %12.3f %13.6e %13.6e %13.6e %13.6e %13.6e %13.6e\n", $zd[$i], $ps[$i], $Ts[$i], $airs[$i], $dens[$i], $o2s[$i], $h2os[$i], $co2s[$i], $no2s[$i];
    }
    close(OUT)
}

sub print_usage()
{
  print "\nScales the stratospheric ozone above a specified\n";
  print "level so that the full ozone column conforms to the\n";
  print "specified ozone column\n";
  print "\n";
  print "Usage: perl ScaleStratosphericOzone.pl [-aozn]\n";
  print " -h prints this message.\n";
  print " -a input atmospheric file, full path\n";
  print " -o ozone column\n";
  print " -z lower altitude above which scaling is done\n";
  print " -n output atmospheric file\n";
  print "\n";
  print "example:  perl ScaleStratosphericOzone.pl -a ../data/atmmod/afglus.dat -o 200 -z 10. -n atmtmp.dat\n";
  print "\n";
}
    
$opt_a="../data/atmmod/afglus.dat";
$opt_o=300;
$opt_z=10.;
$opt_n="./afglus.dat_tmp";
$opt_n="tmp.dat";

$ok = getopts('a:o:z:n:');
if (!$ok) {
    print "invalid input\n";
    exit(0);
}

ScaleStratosphericOzone($opt_a, $opt_o, $opt_z, $opt_n);
