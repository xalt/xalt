#------------------------------------------------------------------------    #
# This python script gets executable usage on SYSHOST grouped by exec name.  #
# Total CPU time used, number of jobs, and number of users of the exec are   #
# shown. Executable with "known" names are shown as all CAPS and grouped     #
# together even if they have different actual exec name / version, other     #
# executables are only grouped by their name.                                # 
#                                                                            #
# Examples:                                                                  #
#                                                                            #
# 1. Show the help output:                                                   #
#   python executable_usage.py -h                                            #
#                                                                            #
# 2. Get executable usage on Darter for the last 90 days                     #
#   python executable_usage.py darter                                        #
#                                                                            #
# 3. Get executable usage on Darter for specific period                      #
#   python executable_usage.py darter --startdate 2015-03-01 \               #
#          --endate 2015-06-31                                               #
#                                                                            #
# 4. Get executable usage on Darter for the last 90 days, excluding all      #
#    module name with 'ari', 'gcc', and 'craype' in its name                 #
#   python executable_usage.py darter --exclude ari,gcc,craype               #
#                                                                            #
#----------------------------------------------------------------------------#

from __future__ import print_function
import os, sys, re, base64, operator
import MySQLdb, argparse
import time
from datetime import datetime, timedelta
try:
  import configparser
except:
  import ConfigParser as configparser

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR","./")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")

parser = argparse.ArgumentParser \
          (description='Get library usage on SYSHOST grouped by module name.\
           The number of linking instances and unique user for each library \
           is displayed. A second table where library version (module name \
           versions) has been agregated is also displayed (assuming module \
           name version is in the form <module_name/version>).')
parser.add_argument \
          ("syshost", metavar="SYSHOST", action="store", \
           help="The syshost for this query.")
parser.add_argument \
          ("--startdate", dest='startdate', action="store", \
           help="exclude everything before STARTDATE (in YYYY-MM-DD). \
                 If not specified, defaults to ENDDATE - 90 days.")
parser.add_argument \
          ("--enddate", dest='enddate', action="store", \
           help="exclude everything after ENDDATE (in YYYY-MM-DD). \
                 If not specified, defaults to today.")
parser.add_argument \
          ("--exclude", dest='patterns', action="store", \
           help="comma separated PATTERN to ignore module name whose \
                 substring matches any of the PATTERNS.")

args = parser.parse_args()

enddate = time.strftime('%Y-%m-%d')
if args.enddate is not None:
  enddate = args.enddate

startdate = (datetime.strptime(enddate, "%Y-%m-%d") - timedelta(90)) \
             .strftime('%Y-%m-%d');
if args.startdate is not None:
  startdate = args.startdate
  
excludePatterns = None
if args.patterns is not None:
  excludePatterns = [x.strip() for x in args.patterns.split(',')]
  
config = configparser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()

equiv_patternA = [
    [ r'^1690'                          , '1690*.x*'                       ],
    [ r'^2d_needle'                     , '2D_needle*'                     ],
    [ r'^3d_needle'                     , '3D_needle*'                     ],
    [ r'^a01'                           , 'A0*'                            ],
    [ r'^abinit'                        , 'Abinit*'                        ],
    [ r'^adcirc'                        , 'ADCIRC*'                        ],
    [ r'^padcswan'                      , 'ADCIRC*'                        ],
    [ r'^padcirc'                       , 'ADCIRC*'                        ],
    [ r'^adf\.exe'                      , 'ADF*'                           ],
    [ r'^ahf-v1\.0'                     , 'AHF-v1.0*'                      ],
    [ r'^arps'                          , 'ARPS*'                          ],
    [ r'^arps_mpi'                      , 'ARPS*'                          ],
    [ r'^pmemd'                         , 'Amber*'                         ],
    [ r'^sander'                        , 'Amber*'                         ],
    [ r'^analysis016'                   , 'Analysis016*'                   ],
    [ r'^baddi3'                        , 'BADDI3*'                        ],
    [ r'^absorption\.real\.x'           , 'BerkeleyGW*'                    ],
    [ r'^absorption\.cplx\.x'           , 'BerkeleyGW*'                    ],
    [ r'^epsilon\.real\.x'              , 'BerkeleyGW*'                    ],
    [ r'^epsilon\.cplx\.x'              , 'BerkeleyGW*'                    ],
    [ r'^kernel\.real\.x'               , 'BerkeleyGW*'                    ],
    [ r'^kernel\.cplx\.x'               , 'BerkeleyGW*'                    ],
    [ r'^sapo\.real\.x'                 , 'BerkeleyGW*'                    ],
    [ r'^sapo\.cplx\.x'                 , 'BerkeleyGW*'                    ],
    [ r'^sigma\.real\.x'                , 'BerkeleyGW*'                    ],
    [ r'^sigma\.cplx\.x'                , 'BerkeleyGW*'                    ],
    [ r'^camx'                          , 'CAMx*'                          ],
    [ r'^cfs_ncep_post'                 , 'CFSv2*'                         ],
    [ r'^cesm.exe'                      , 'CESM*'                          ],
    [ r'^c37b1'                         , 'CHARMM*'                        ],
    [ r'^charmm'                        , 'CHARMM*'                        ],
    [ r'^chns'                          , 'CHNS*'                          ],
    [ r'^run\.cctm'                     , 'CMAQ_CCTM*'                     ],
    [ r'^cactus_'                       , 'Cactus*'                        ],
    [ r'^chim'                          , 'CHIMERA*'                       ],
    [ r'^chroma'                        , 'Chroma*'                        ],
    [ r'^harom\.parscalar'              , 'Chroma*'                        ],
    [ r'^sm_chroma'                     , 'Chroma*'                        ],
    [ r'^citcoms'                       , 'CitcomS*'                       ],
    [ r'^cosmomc'                       , 'CosmoMC*'                       ],
    [ r'^comd-'                         , 'CoMD*'                          ],
    [ r'^compute_'                      , 'Compute_Weather_Code*'          ],
    [ r'^cpmd\.x'                       , 'CPMD*'                          ],
    [ r'^cp2k'                          , 'CP2K*'                          ],
    [ r'^daddi_'                        , 'DADDI*'                         ],
    [ r'^mpi_dbscan'                    , 'DBScan*'                        ],
    [ r'^dcmip'                         , 'DCMIP*'                         ],
    [ r'^dlpoly'                        , 'DL_POLY*'                       ],
    [ r'^dns2d'                         , 'DNS2d*'                         ],
    [ r'^enkf\.mpi'                     , 'EnKF*'                          ],
    [ r'^global_enkf\.x'                , 'EnKF*'                          ],
    [ r'^enzo'                          , 'ENZO*'                          ],
    [ r'^fd3d'                          , 'FD3D*'                          ],
    [ r'^fin'                           , 'FIN*'                           ],
    [ r'^fphc'                          , 'FPHC*'                          ],
    [ r'^flash3'                        , 'Flash3*'                        ],
    [ r'^flash4'                        , 'Flash4*'                        ],
    [ r'^floating_jinhui'               , 'Floating_Jinhui*'               ],
    [ r'^fractionalvaporsaturationtime' , 'FractionalVaporSaturationTime*' ],
    [ r'^frankwolfe'                    , 'FrankWolfe*'                    ],     
    [ r'^fvcom'                         , 'FVCOM*'                         ],     
    [ r'^gizmo'                         , 'GIZMO*'                         ],
    [ r'^gsreturn'                      , 'GSreturn*'                      ],
    [ r'^.*gadget.*'                    , 'Gadget*'                        ],
    [ r'^g09'                           , 'Gaussian*'                      ],
    [ r'^gigapowers'                    , 'GigaPOWERS*'                    ],
    [ r'^graph500'                      , 'Graph500*'                      ],
    [ r'^mdrun'                         , 'Gromacs*'                       ],
    [ r'^gmx_mpi'                       , 'Gromacs*'                       ],
    [ r'^hf'                            , 'HF*'                            ],
    [ r'^hoomd'                         , 'HOOMD*'                         ],
    [ r'^hopspack_-'                    , 'HOPSPACK*'                      ],
    [ r'^xhpcg'                         , 'HPCG*'                          ],
    [ r'^xhpl'                          , 'HPL*'                           ],
    [ r'^IMB-'                          , 'IMB*'                           ],
    [ r'^icenine'                       , 'IceNine*'                       ],
    [ r'^lmp_'                          , 'LAMMPS*'                        ],
    [ r'^lespdfisat'                    , 'LESPDFISAT*'                    ],
    [ r'^lulesh'                        , 'LULESH*'                        ],
    [ r'^longitudinal-'                 , 'Longitudinal*'                  ],
    [ r'^meep-mpi'                      , 'MEEP*'                          ],
    [ r'^meme_p'                        , 'MEME*'                          ],
    [ r'^mhdam3d_'                      , 'MHDAM3d*'                       ],
    [ r'^su3_'                          , 'MILC*'                          ],
    [ r'^mitgcmuv'                      , 'MITGCM*'                        ],
    [ r'^charmrun'                      , 'NAMD*'                          ],
    [ r'^namd2'                         , 'NAMD*'                          ],
    [ r'^nektar'                        , 'NEKTAR*'                        ],
    [ r'^nrlmol'                        , 'NRLMol*'                        ],
    [ r'^NVT_'                          , 'NVT*'                           ],
    [ r'^nwchem'                        , 'NWChem*'                        ],
    [ r'^navstk_'                       , 'NavStk*'                        ],
    [ r'^NewAlgorithm'                  , 'NewAlgorithm*'                  ],
    [ r'^O2case'                        , 'O2case*'                        ],
    [ r'^octopus_mpi'                   , 'Octopus*'                       ],
    [ r'^.*foam.*'                      , 'OpenFOAM*'                      ],
    [ r'^abslsolver'                    , 'OpenFOAM*'                      ],
    [ r'^openmx'                        , 'OPENMX*'                        ],
    [ r'^opensees'                      , 'OpenSees*'                      ],
    [ r'^openseessp'                    , 'OpenSees*'                      ],
    [ r'^p-sph'                         , 'P-SPH*'                         ],
    [ r'^p_merge_ld'                    , 'P_merge_LD*'                    ],
    [ r'^pvserver'                      , 'Paraview*'                      ],
    [ r'^parsec.mpi'                    , 'PARSEC*'                        ],
    [ r'^peridigm'                      , 'Peridigm*'                      ],
    [ r'^plascomcm'                     , 'PlasComCM*'                     ],
    [ r'^i?python'                      , 'Python*'                        ],
    [ r'^qcprog'                        , 'QCHEM*'                         ],
    [ r'^bands\.x'                      , 'QE*'                            ],
    [ r'^neb\.x'                        , 'QE*'                            ],
    [ r'^pw2wannier90\.x'               , 'QE*'                            ],
    [ r'^projwfc.x'                     , 'QE*'                            ],
    [ r'^ph\.x'                         , 'QE*'                            ],
    [ r'^pw\.x'                         , 'QE*'                            ],
    [ r'^relion_refine_mpi'             , 'Relion*'                        ],
    [ r'^runrsqsim'                     , 'RSQSim*'                        ],
    [ r'^rscript'                       , 'Rstats*'                        ],
    [ r'^rosetta'                       , 'Rosetta*'                       ],
    [ r'^rmpisnow'                      , 'Rstats*'                        ],
    [ r'^minirosetta\.mpi\.linux'       , 'Rosetta*'                       ],
    [ r'^docking_protocol\.mpi\.linux'  , 'Rosetta*'                       ],
    [ r'^rosetta'                       , 'Rosetta*'                       ],
    [ r'^sam_adv_'                      , 'SAM_ADV*'                       ],
    [ r'^ssepbmpi_3d_'                  , 'SSEPBMPI_3d*'                   ],
    [ r'^sandia_100_'                   , 'Sandia_100*'                    ],
    [ r'^sbm_lfp_'                      , 'Sbm_LFP*'                       ],
    [ r'^sesisol'                       , 'SeisSol*'                       ],
    [ r'^select_particles'              , 'Select_Particles*'              ],
    [ r'^sept1'                         , 'Sept1*'                         ],
    [ r'^siesta'                        , 'Siesta*'                        ],
    [ r'^spacing'                       , 'Spacing*'                       ],
    [ r'^xmeshfem3D'                    , 'SpecFEM3D*'                     ],
    [ r'^xspecfem3D'                    , 'SpecFEM3D*'                     ],
    [ r'^splotch'                       , 'Splotch*'                       ],
    [ r'^stratified_'                   , 'Stratified*'                    ],
    [ r'^ttmmd'                         , 'TTMMD*'                         ],
    [ r'^md.exe'                        , 'TTMMD*'                         ],
    [ r'^taddi'                         , 'TADDI*'                         ],
    [ r'^t_matrix'                      , 'T_Matrix*'                      ],
    [ r'^trispectrum'                   , 'Trispectrum*'                   ],
    [ r'^sus'                           , 'Uintah*'                        ],
    [ r'^ut-gmres'                      , 'UT-GMRES*'                      ],
    [ r'^ut-mom'                        , 'UT-MOM*'                        ],
    [ r'^UT.*AIM'                       , 'UTAIM*'                         ],
    [ r'[0-9]+_[0-9]+.sh'               , 'Unknown_number_pair.sh@'        ],
    [ r'^vasp'                          , 'VASP*'                          ],          
    [ r'^wps_'                          , 'WPS*'                           ],
    [ r'^da_update_bc.exe'              , 'WRF*'                           ],
    [ r'^da_wrfvar.exe'                 , 'WRF*'                           ],
    [ r'^global_enkf_wrf'               , 'WRF*'                           ],
    [ r'^ndown.exe'                     , 'WRF*'                           ],
    [ r'^real.exe'                      , 'WRF*'                           ],
    [ r'^metgrid.exe'                   , 'WRF*'                           ],
    [ r'^wrf'                           , 'WRF*'                           ],
    [ r'^read_wrfnetcdf'                , 'WRF*'                           ],
    [ r'^Xvicar'                        , 'Xvicar*'                        ],
    [ r'^a\.out'                        , 'a.out'                          ],
    [ r'ablDyM'                         , 'ablDyM*'                        ],
    [ r'^ah1w'                          , 'ah1w*'                          ],
    [ r'^astrobear'                     , 'astrobear*'                     ],
    [ r'^athena'                        , 'athena*'                        ],
    [ r'^bsr'                           , 'bsr*'                           ],
    [ r'^buoyantBoussinesq'             , 'buoyantBoussines*'              ],
    [ r'^.*cdmft'                       , 'cdmft*'                         ],
    [ r'^citcom'                        , 'citcom*'                        ],
    [ r'^cntor'                         , 'cntor*'                         ],
    [ r'^coawst'                        , 'coawst*'                        ],
    [ r'^com_estimator'                 , 'com_estimator*'                 ],
    [ r'^cp2k\.'                        , 'cp2k*'                          ],
    [ r'^dam_'                          , 'dam*'                           ],
    [ r'^deform_lung'                   , 'deform_lung*'                   ],
    [ r'^dgbte_'                        , 'dgbte*'                         ],
    [ r'^dgsg'                          , 'dgsg*'                          ],
    [ r'^epoch2d'                       , 'epoch*'                         ],
    [ r'^equm-'                         , 'equm*'                          ],
    [ r'^etbm_hartree'                  , 'etbm_hartree*'                  ],
    [ r'^fdtd-engine'                   , 'fdtd-engine*'                   ],
    [ r'^fkqcwl'                        , 'fkqcwl*'                        ],
    [ r'^flw-avni'                      , 'fl2-avni*'                      ],
    [ r'^flamelet[A-Z]'                 , 'flamelet*'                      ],
    [ r'^fmm_'                          , 'fmm*'                           ],
    [ r'^grmhd'                         , 'grmhd*'                         ],
    [ r'^harris2d'                      , 'harris2d*'                      ],
    [ r'^himeno'                        , 'himeno*'                        ],
    [ r'^hpcrun'                        , 'hpctoolkit*'                    ],
    [ r'^iblank'                        , 'iblank*'                        ],
    [ r'^ioChann'                       , 'ioChann*'                       ],
    [ r'^ks_spectrum'                   , 'ks_spectrum*'                   ],
    [ r'^laminarSMOKE'                  , 'laminarSMOKE*'                  ],
    [ r'^lassopg'                       , 'lassopg*'                       ],
    [ r'^lbs3d'                         , 'lbs3d*'                         ],
    [ r'^lchgall_'                      , 'lchgall*'                       ],
    [ r'^lesmpi_rankine'                , 'lesmpi_rankine*'                ],
    [ r'^lz_'                           , 'lz*'                            ],
    [ r'^mandel_'                       , 'mandel*'                        ],
    [ r'^mcmc_test'                     , 'mcmc*'                          ],
    [ r'^md3d_'                         , 'md3d*'                          ],
    [ r'^melSplit'                      , 'melSplit*'                      ],
    [ r'^mh1w'                          , 'mh1w*'                          ],
    [ r'^mhray_'                        , 'mhray*'                         ],
    [ r'^molpairs'                      , 'molpairs*'                      ],
    [ r'^mpiblast'                      , 'mpiblast*'                      ],
    [ r'^ncl'                           , 'ncl_ncarg*'                     ],
    [ r'[0-9][0-9]*.sh'                 , 'number.sh@'                     ],
    [ r'^ocean[MG]'                     , 'ocean*'                         ],
    [ r'^pelfe_'                        , 'pelfe*'                         ],
    [ r'^perf_cfft'                     , 'perf_cfft*'                     ],
    [ r'^piso'                          , 'piso*'                          ],
    [ r'^polaris_'                      , 'polaris*'                       ],
    [ r'^ramses'                        , 'ramses*'                        ],
    [ r'^ranksort_'                     , 'ranksort*'                      ],
    [ r'^rfcst_'                        , 'rfcst*'                         ],
    [ r'^rocflocm'                      , 'rocflocm*'                      ],
    [ r'^run\.cctm'                     , 'run.cctm*'                      ],
    [ r'^run\.ddm'                      , 'run.ddm*'                       ],
    [ r'^run_3ln'                       , 'run_3ln*'                       ],
    [ r'^run_aedtproc'                  , 'run_aedtproc*'                  ],
    [ r'^sem_model_slice'               , 'sem_model_slice*'               ],
    [ r'^sfmpi'                         , 'sfmpi*'                         ],
    [ r'^smk_'                          , 'smk*'                           ],
    [ r'^sp-mz'                         , 'sp-mz*'                         ],
    [ r'^spmv-'                         , 'spmv*'                          ],
    [ r'^sssp_eval-'                    , 'sssp_eval*'                     ],
    [ r'^toascii.*ksh'                  , 'toascii*'                       ],
    [ r'^v14'                           , 'v14*'                           ],
    [ r'^validation[a-z0-9]'            , 'validation*'                    ],
    [ r'^varOmega'                      , 'varOmega*'                      ],
    [ r'^vat_2d'                        , 'vat_2d*'                        ],
    [ r'^vat_3d'                        , 'vat_3d*'                        ],
    [ r'^vfmfe'                         , 'vfmfe*'                         ],
    [ r'^virial3'                       , 'virial3*'                       ],
    [ r'^vlpl'                          , 'vlpl*'                          ],
    [ r'^xkhi03'                        , 'xkhi03*'                        ],
    [ r'^xlmg1'                         , 'xlmg1*'                         ],
    [ r'^xpacc'                         , 'xpacc*'                         ],
    [ r'^xsbig'                         , 'xsbig*'                         ],
    [ r'^ymir_'                         , 'ymir*'                          ],
    ]

sA = []
sA.append("SELECT CASE ")
for entry in equiv_patternA:
  left  = entry[0].lower()
  right = entry[1]
  s     = "WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP '%s' then '%s' " % (left, right)
  sA.append(s)

sA.append(" ELSE SUBSTRING_INDEX(xalt_run.exec_path,'/',-1) END ")
sA.append(" AS execname, ROUND(SUM(run_time*num_cores/3600)) as totalcput, ")
sA.append(" COUNT(date) as n_jobs, COUNT(DISTINCT(user)) as n_users ")
sA.append("   FROM xalt_run ")
sA.append("  WHERE syshost = '%s' ")
sA.append("    AND date >= '%s 00:00:00' AND date <= '%s 23:59:59' ")
sA.append("  GROUP BY execname ORDER BY totalcput DESC")

query = "".join(sA) % (args.syshost, startdate, enddate)

cursor.execute(query)
results = cursor.fetchall()

print ("")
print ("====================================================================")
print ("%10s %10s %10s %s" % ("CPU Time.", "# Jobs", "# Users","Exec"))
print ("====================================================================")

sum = 0.0
for execname, totalcput, n_jobs, n_users in results:
  sum += totalcput
  print ("%10s %10s %10s %s" % (totalcput, n_jobs, n_users,execname))

print("(M) SUs", sum/1.0e6)

