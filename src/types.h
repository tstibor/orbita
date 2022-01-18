#ifndef TYPES_H
#define TYPES_H

#include <QtGlobal>

struct asteroid_t {
    char	designation[8];	          /* Designation. */
    double	H;		          /* Absolute magnitude. */
    double	G;		          /* Slope parameter. */
    char	epoch[10]; 	          /* Epoch (in form YYYY-MM-DD) */
    double	anomaly_mean;	          /* Mean anomaly at the epoch, in degrees. */
    double	perihelion;	          /* Argument of perihelion, J2000.0 (degrees). */
    double	ascending_node;	          /* Longitude of the ascending node, J2000.0 (degrees). */
    double	inclination;	          /* Inclination to the ecliptic, J2000.0 (degrees). */
    double	eccentricity;	          /* Orbital eccentricity. */
    double	daily_motion;	          /* Mean daily motion (degrees per day). */
    double	semimajor_axis;           /* Semimajor axis (AU). */
    quint32     observations;	          /* Number of observations. */
    quint16     classification_flag;	  /* Classification of distant-orbit types. */
    char        designation_readable[29]; /* Readable designation. */
    char        last_observation[8];      /* Date of last observation included in orbit solution (YYYYMMDD format). */
};

struct comet_t {
    char        designation_name[56]; /* Designation and Name.*/
    char        orbit_type[16];	      /* Orbit type. */
    quint16     perihelion_year;      /* Year of perihelion passage. */
    quint8      perihelion_month;     /* Month of perihelion passage. */
    double      perihelion_day;	      /* Day of perihelion passage (TT). */
    double      perihelion_distance;  /* Perihelion distance (AU). */
    double      eccentricity;	      /* Orbital eccentricity. */
    double      perihelion;	      /* Argument of perihelion, J2000.0 (degrees). */
    double      ascending_node;	      /* Longitude of the ascending node, J2000.0 (degrees). */
    double      inclination;	      /* Inclination in degrees, J2000.0 (degrees). */
    quint16     epoch_pert_year;      /* Year of epoch for perturbed solutions. */
    quint8      epoch_pert_month;     /* Month of epoch for perturbed solutions. */
    quint8      epoch_pert_day;       /* Day of epoch for perturbed solutions. */
    double      H;		      /* Absolute magnitude. */
    double      G;		      /* Slope parameter. */
};

/*
  Note according to https://www.minorplanetcenter.net/iau/info/OpticalObs.html
  orbit_type values are (where 'I' is added by myself):

  'C' for a long-period comet,
  'P' for a short-period comet,
  'D' for a defunct comet,
  'X' for an uncertain comet,
  'I' for an interstellar comet,
  'A' for a minor planet given a cometary designation.
 */

#endif /* TYPES_H */
