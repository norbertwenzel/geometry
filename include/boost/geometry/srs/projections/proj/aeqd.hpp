#ifndef BOOST_GEOMETRY_PROJECTIONS_AEQD_HPP
#define BOOST_GEOMETRY_PROJECTIONS_AEQD_HPP

// Boost.Geometry - extensions-gis-projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright (c) 2008-2015 Barend Gehrels, Amsterdam, the Netherlands.

// This file was modified by Oracle on 2017.
// Modifications copyright (c) 2017, Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Boost.Geometry by Barend Gehrels

// Last updated version of proj: 4.9.1

// Original copyright notice:

// Purpose:  Implementation of the aeqd (Azimuthal Equidistant) projection.
// Author:   Gerald Evenden
// Copyright (c) 1995, Gerald Evenden

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <boost/geometry/util/math.hpp>
#include <boost/math/special_functions/hypot.hpp>

#include <boost/geometry/srs/projections/impl/base_static.hpp>
#include <boost/geometry/srs/projections/impl/base_dynamic.hpp>
#include <boost/geometry/srs/projections/impl/projects.hpp>
#include <boost/geometry/srs/projections/impl/factory_entry.hpp>
#include <boost/geometry/srs/projections/impl/aasincos.hpp>
#include <boost/geometry/srs/projections/impl/pj_mlfn.hpp>

#include <boost/geometry/srs/projections/par4.hpp>

#include <boost/type_traits/is_same.hpp>

namespace boost { namespace geometry
{

namespace srs { namespace par4
{
    struct aeqd {};
    //struct aeqd_guam {};

}} //namespace srs::par4

namespace projections
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace aeqd
    {

            static const double EPS10 = 1.e-10;
            static const double TOL = 1.e-14;
            static const int N_POLE = 0;
            static const int S_POLE = 1;
            static const int EQUIT = 2;
            static const int OBLIQ = 3;

            template <typename T>
            struct par_aeqd
            {
                T    sinph0;
                T    cosph0;
                T    en[EN_SIZE];
                T    M1;
                T    N1;
                T    Mp;
                T    He;
                T    G;
                int        mode;
            };

            template <typename T, typename Par, typename ProjParm>
            inline void e_forward(T& lp_lon, T& lp_lat, T& xy_x, T& xy_y, Par const& par, ProjParm const& proj_parm)
            {
                T  coslam, cosphi, sinphi, rho, s, H, H2, c, Az, t, ct, st, cA, sA;

                coslam = cos(lp_lon);
                cosphi = cos(lp_lat);
                sinphi = sin(lp_lat);
                switch (proj_parm.mode) {
                case N_POLE:
                    coslam = - coslam;
                case S_POLE:
                    xy_x = (rho = fabs(proj_parm.Mp - pj_mlfn(lp_lat, sinphi, cosphi, proj_parm.en))) *
                        sin(lp_lon);
                    xy_y = rho * coslam;
                    break;
                case EQUIT:
                case OBLIQ:
                    if (fabs(lp_lon) < EPS10 && fabs(lp_lat - par.phi0) < EPS10) {
                        xy_x = xy_y = 0.;
                        break;
                    }
                    t = atan2(par.one_es * sinphi + par.es * proj_parm.N1 * proj_parm.sinph0 *
                        sqrt(1. - par.es * sinphi * sinphi), cosphi);
                    ct = cos(t); st = sin(t);
                    Az = atan2(sin(lp_lon) * ct, proj_parm.cosph0 * st - proj_parm.sinph0 * coslam * ct);
                    cA = cos(Az); sA = sin(Az);
                    s = aasin(fabs(sA) < TOL ?
                        (proj_parm.cosph0 * st - proj_parm.sinph0 * coslam * ct) / cA :
                        sin(lp_lon) * ct / sA );
                    H = proj_parm.He * cA;
                    H2 = H * H;
                    c = proj_parm.N1 * s * (1. + s * s * (- H2 * (1. - H2)/6. +
                        s * ( proj_parm.G * H * (1. - 2. * H2 * H2) / 8. +
                        s * ((H2 * (4. - 7. * H2) - 3. * proj_parm.G * proj_parm.G * (1. - 7. * H2)) /
                        120. - s * proj_parm.G * H / 48.))));
                    xy_x = c * sA;
                    xy_y = c * cA;
                    break;
                }
            }

            template <typename T, typename Par, typename ProjParm>
            inline void e_inverse(T& xy_x, T& xy_y, T& lp_lon, T& lp_lat, Par const& par, ProjParm const& proj_parm)
            {
                static const T HALFPI = detail::HALFPI<T>();

                T c, Az, cosAz, A, B, D, E, F, psi, t;

                if ((c = boost::math::hypot(xy_x, xy_y)) < EPS10) {
                    lp_lat = par.phi0;
                    lp_lon = 0.;
                        return;
                }
                if (proj_parm.mode == OBLIQ || proj_parm.mode == EQUIT) {
                    cosAz = cos(Az = atan2(xy_x, xy_y));
                    t = proj_parm.cosph0 * cosAz;
                    B = par.es * t / par.one_es;
                    A = - B * t;
                    B *= 3. * (1. - A) * proj_parm.sinph0;
                    D = c / proj_parm.N1;
                    E = D * (1. - D * D * (A * (1. + A) / 6. + B * (1. + 3.*A) * D / 24.));
                    F = 1. - E * E * (A / 2. + B * E / 6.);
                    psi = aasin(proj_parm.sinph0 * cos(E) + t * sin(E));
                    lp_lon = aasin(sin(Az) * sin(E) / cos(psi));
                    if ((t = fabs(psi)) < EPS10)
                        lp_lat = 0.;
                    else if (fabs(t - HALFPI) < 0.)
                        lp_lat = HALFPI;
                    else
                        lp_lat = atan((1. - par.es * F * proj_parm.sinph0 / sin(psi)) * tan(psi) /
                            par.one_es);
                } else { /* Polar */
                    lp_lat = pj_inv_mlfn(proj_parm.mode == N_POLE ? proj_parm.Mp - c : proj_parm.Mp + c,
                        par.es, proj_parm.en);
                    lp_lon = atan2(xy_x, proj_parm.mode == N_POLE ? -xy_y : xy_y);
                }
            }

            template <typename T, typename Par, typename ProjParm>
            inline void e_guam_fwd(T& lp_lon, T& lp_lat, T& xy_x, T& xy_y, Par const& par, ProjParm const& proj_parm)
            {
                T cosphi, sinphi, t;

                cosphi = cos(lp_lat);
                sinphi = sin(lp_lat);
                t = 1. / sqrt(1. - par.es * sinphi * sinphi);
                xy_x = lp_lon * cosphi * t;
                xy_y = pj_mlfn(lp_lat, sinphi, cosphi, proj_parm.en) - proj_parm.M1 +
                    .5 * lp_lon * lp_lon * cosphi * sinphi * t;
            }

            template <typename T, typename Par, typename ProjParm>
            inline void e_guam_inv(T& xy_x, T& xy_y, T& lp_lon, T& lp_lat, Par const& par, ProjParm const& proj_parm)
            {
                T x2, t;
                int i;

                x2 = 0.5 * xy_x * xy_x;
                lp_lat = par.phi0;
                for (i = 0; i < 3; ++i) {
                    t = par.e * sin(lp_lat);
                    lp_lat = pj_inv_mlfn(proj_parm.M1 + xy_y -
                        x2 * tan(lp_lat) * (t = sqrt(1. - t * t)), par.es, proj_parm.en);
                }
                lp_lon = xy_x * t / cos(lp_lat);
            }

            template <typename T, typename Par, typename ProjParm>
            inline void s_forward(T& lp_lon, T& lp_lat, T& xy_x, T& xy_y, Par const& par, ProjParm const& proj_parm)
            {
                static const T HALFPI = detail::HALFPI<T>();
                    
                T coslam, cosphi, sinphi;

                sinphi = sin(lp_lat);
                cosphi = cos(lp_lat);
                coslam = cos(lp_lon);
                switch (proj_parm.mode) {
                case EQUIT:
                    xy_y = cosphi * coslam;
                    goto oblcon;
                case OBLIQ:
                    xy_y = proj_parm.sinph0 * sinphi + proj_parm.cosph0 * cosphi * coslam;
            oblcon:
                    if (fabs(fabs(xy_y) - 1.) < TOL)
                        if (xy_y < 0.)
                            BOOST_THROW_EXCEPTION( projection_exception(-20) );
                        else
                            xy_x = xy_y = 0.;
                    else {
                        xy_y = acos(xy_y);
                        xy_y /= sin(xy_y);
                        xy_x = xy_y * cosphi * sin(lp_lon);
                        xy_y *= (proj_parm.mode == EQUIT) ? sinphi :
                                proj_parm.cosph0 * sinphi - proj_parm.sinph0 * cosphi * coslam;
                    }
                    break;
                case N_POLE:
                    lp_lat = -lp_lat;
                    coslam = -coslam;
                case S_POLE:
                    if (fabs(lp_lat - HALFPI) < EPS10)
                        BOOST_THROW_EXCEPTION( projection_exception(-20) );
                    xy_x = (xy_y = (HALFPI + lp_lat)) * sin(lp_lon);
                    xy_y *= coslam;
                    break;
                }
            }

            template <typename T, typename Par, typename ProjParm>
            inline void s_inverse(T& xy_x, T& xy_y, T& lp_lon, T& lp_lat, Par const& par, ProjParm const& proj_parm)
            {
                static const T ONEPI = detail::ONEPI<T>();
                static const T HALFPI = detail::HALFPI<T>();
                    
                T cosc, c_rh, sinc;

                if ((c_rh = boost::math::hypot(xy_x, xy_y)) > ONEPI) {
                    if (c_rh - EPS10 > ONEPI)
                        BOOST_THROW_EXCEPTION( projection_exception(-20) );
                    c_rh = ONEPI;
                } else if (c_rh < EPS10) {
                    lp_lat = par.phi0;
                    lp_lon = 0.;
                        return;
                }
                if (proj_parm.mode == OBLIQ || proj_parm.mode == EQUIT) {
                    sinc = sin(c_rh);
                    cosc = cos(c_rh);
                    if (proj_parm.mode == EQUIT) {
                                    lp_lat = aasin(xy_y * sinc / c_rh);
                        xy_x *= sinc;
                        xy_y = cosc * c_rh;
                    } else {
                        lp_lat = aasin(cosc * proj_parm.sinph0 + xy_y * sinc * proj_parm.cosph0 /
                            c_rh);
                        xy_y = (cosc - proj_parm.sinph0 * sin(lp_lat)) * c_rh;
                        xy_x *= sinc * proj_parm.cosph0;
                    }
                    lp_lon = atan2(xy_x, xy_y);
                } else if (proj_parm.mode == N_POLE) {
                    lp_lat = HALFPI - c_rh;
                    lp_lon = atan2(xy_x, -xy_y);
                } else {
                    lp_lat = c_rh - HALFPI;
                    lp_lon = atan2(xy_x, xy_y);
                }
            }

            // Azimuthal Equidistant
            template <typename Parameters, typename T>
            inline void setup_aeqd(Parameters& par, par_aeqd<T>& proj_parm, bool is_sphere, bool is_guam)
            {
                static const T HALFPI = detail::HALFPI<T>();

                par.phi0 = pj_param(par.params, "rlat_0").f;
                if (fabs(fabs(par.phi0) - HALFPI) < EPS10) {
                    proj_parm.mode = par.phi0 < 0. ? S_POLE : N_POLE;
                    proj_parm.sinph0 = par.phi0 < 0. ? -1. : 1.;
                    proj_parm.cosph0 = 0.;
                } else if (fabs(par.phi0) < EPS10) {
                    proj_parm.mode = EQUIT;
                    proj_parm.sinph0 = 0.;
                    proj_parm.cosph0 = 1.;
                } else {
                    proj_parm.mode = OBLIQ;
                    proj_parm.sinph0 = sin(par.phi0);
                    proj_parm.cosph0 = cos(par.phi0);
                }
                if (is_sphere) {
                } else {
                    if (!pj_enfn(par.es, proj_parm.en))
                        BOOST_THROW_EXCEPTION( projection_exception(0) );
                    if (is_guam) {
                        proj_parm.M1 = pj_mlfn(par.phi0, proj_parm.sinph0, proj_parm.cosph0, proj_parm.en);
                    } else {
                        switch (proj_parm.mode) {
                        case N_POLE:
                            proj_parm.Mp = pj_mlfn<T>(HALFPI, 1., 0., proj_parm.en);
                            break;
                        case S_POLE:
                            proj_parm.Mp = pj_mlfn<T>(-HALFPI, -1., 0., proj_parm.en);
                            break;
                        case EQUIT:
                        case OBLIQ:
                            proj_parm.N1 = 1. / sqrt(1. - par.es * proj_parm.sinph0 * proj_parm.sinph0);
                            proj_parm.G = proj_parm.sinph0 * (proj_parm.He = par.e / sqrt(par.one_es));
                            proj_parm.He *= proj_parm.cosph0;
                            break;
                        }
                    }
                }
            }

            // template class, using CRTP to implement forward/inverse
            template <typename CalculationType, typename Parameters>
            struct base_aeqd_e : public base_t_fi<base_aeqd_e<CalculationType, Parameters>,
                     CalculationType, Parameters>
            {

                typedef CalculationType geographic_type;
                typedef CalculationType cartesian_type;

                par_aeqd<CalculationType> m_proj_parm;

                inline base_aeqd_e(const Parameters& par)
                    : base_t_fi<base_aeqd_e<CalculationType, Parameters>,
                     CalculationType, Parameters>(*this, par) {}

                // FORWARD(e_forward)  elliptical
                // Project coordinates from geographic (lon, lat) to cartesian (x, y)
                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    e_forward(lp_lon, lp_lat, xy_x, xy_y, this->m_par, this->m_proj_parm);
                }

                // INVERSE(e_inverse)  elliptical
                // Project coordinates from cartesian (x, y) to geographic (lon, lat)
                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    e_inverse(xy_x, xy_y, lp_lon, lp_lat, this->m_par, this->m_proj_parm);
                }

                static inline std::string get_name()
                {
                    return "aeqd_e";
                }

            };

            // template class, using CRTP to implement forward/inverse
            template <typename CalculationType, typename Parameters>
            struct base_aeqd_e_guam : public base_t_fi<base_aeqd_e_guam<CalculationType, Parameters>,
                     CalculationType, Parameters>
            {

                typedef CalculationType geographic_type;
                typedef CalculationType cartesian_type;

                par_aeqd<CalculationType> m_proj_parm;

                inline base_aeqd_e_guam(const Parameters& par)
                    : base_t_fi<base_aeqd_e_guam<CalculationType, Parameters>,
                     CalculationType, Parameters>(*this, par) {}

                // FORWARD(e_guam_fwd)  Guam elliptical
                // Project coordinates from geographic (lon, lat) to cartesian (x, y)
                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    e_guam_fwd(lp_lon, lp_lat, xy_x, xy_y, this->m_par, this->m_proj_parm);
                }

                // INVERSE(e_guam_inv)  Guam elliptical
                // Project coordinates from cartesian (x, y) to geographic (lon, lat)
                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    e_guam_inv(xy_x, xy_y, lp_lon, lp_lat, this->m_par, this->m_proj_parm);
                }

                static inline std::string get_name()
                {
                    return "aeqd_e_guam";
                }

            };

            // template class, using CRTP to implement forward/inverse
            template <typename BGParameters, typename CalculationType, typename Parameters>
            struct base_aeqd_e_static : public base_t_fi<base_aeqd_e_static<BGParameters, CalculationType, Parameters>,
                     CalculationType, Parameters>
            {

                typedef CalculationType geographic_type;
                typedef CalculationType cartesian_type;

                par_aeqd<CalculationType> m_proj_parm;

                static const bool is_guam = ! boost::is_same
                    <
                        typename srs::par4::detail::tuples_find_if
                            <
                                BGParameters,
                                srs::par4::detail::is_param<srs::par4::guam>::is_same
                            >::type,
                        void
                    >::value;

                inline base_aeqd_e_static(const Parameters& par)
                    : base_t_fi<base_aeqd_e_static<BGParameters, CalculationType, Parameters>,
                     CalculationType, Parameters>(*this, par)
                {}

                // FORWARD(e_forward or e_guam_fwd)  elliptical
                // Project coordinates from geographic (lon, lat) to cartesian (x, y)
                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    if (is_guam)
                        e_guam_fwd(lp_lon, lp_lat, xy_x, xy_y, this->m_par, this->m_proj_parm);
                    else
                        e_forward(lp_lon, lp_lat, xy_x, xy_y, this->m_par, this->m_proj_parm);
                }

                // INVERSE(e_inverse or e_guam_inv)  elliptical
                // Project coordinates from cartesian (x, y) to geographic (lon, lat)
                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    if (is_guam)
                        e_guam_inv(xy_x, xy_y, lp_lon, lp_lat, this->m_par, this->m_proj_parm);
                    else
                        e_inverse(xy_x, xy_y, lp_lon, lp_lat, this->m_par, this->m_proj_parm);
                }

                static inline std::string get_name()
                {
                    return "aeqd_e_static";
                }

            };

            // template class, using CRTP to implement forward/inverse
            template <typename CalculationType, typename Parameters>
            struct base_aeqd_s : public base_t_fi<base_aeqd_s<CalculationType, Parameters>,
                     CalculationType, Parameters>
            {

                typedef CalculationType geographic_type;
                typedef CalculationType cartesian_type;

                par_aeqd<CalculationType> m_proj_parm;

                inline base_aeqd_s(const Parameters& par)
                    : base_t_fi<base_aeqd_s<CalculationType, Parameters>,
                     CalculationType, Parameters>(*this, par) {}

                // FORWARD(s_forward)  spherical
                // Project coordinates from geographic (lon, lat) to cartesian (x, y)
                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    s_forward(lp_lon, lp_lat, xy_x, xy_y, this->m_par, this->m_proj_parm);
                }

                // INVERSE(s_inverse)  spherical
                // Project coordinates from cartesian (x, y) to geographic (lon, lat)
                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    s_inverse(xy_x, xy_y, lp_lon, lp_lat, this->m_par, this->m_proj_parm);
                }

                static inline std::string get_name()
                {
                    return "aeqd_s";
                }

            };

    }} // namespace detail::aeqd
    #endif // doxygen

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
        \par Projection parameters
         - lat_0: Latitude of origin (degrees)
         - guam (boolean)
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename CalculationType, typename Parameters>
    struct aeqd_e : public detail::aeqd::base_aeqd_e<CalculationType, Parameters>
    {
        inline aeqd_e(const Parameters& par) : detail::aeqd::base_aeqd_e<CalculationType, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm, false, false);
        }
    };

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
        \par Projection parameters
         - lat_0: Latitude of origin (degrees)
         - guam (boolean)
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename CalculationType, typename Parameters>
    struct aeqd_e_guam : public detail::aeqd::base_aeqd_e_guam<CalculationType, Parameters>
    {
        inline aeqd_e_guam(const Parameters& par) : detail::aeqd::base_aeqd_e_guam<CalculationType, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm, false, true);
        }
    };

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
        \par Projection parameters
         - lat_0: Latitude of origin (degrees)
         - guam (boolean)
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename BGParameters, typename CalculationType, typename Parameters>
    struct aeqd_e_static : public detail::aeqd::base_aeqd_e_static<BGParameters, CalculationType, Parameters>
    {
        inline aeqd_e_static(const Parameters& par) : detail::aeqd::base_aeqd_e_static<BGParameters, CalculationType, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm,
                                     false,
                                     detail::aeqd::base_aeqd_e_static<BGParameters, CalculationType, Parameters>::is_guam);
        }
    };

    /*!
        \brief Azimuthal Equidistant projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
        \par Projection parameters
         - lat_0: Latitude of origin (degrees)
         - guam (boolean)
        \par Example
        \image html ex_aeqd.gif
    */
    template <typename CalculationType, typename Parameters>
    struct aeqd_s : public detail::aeqd::base_aeqd_s<CalculationType, Parameters>
    {
        inline aeqd_s(const Parameters& par) : detail::aeqd::base_aeqd_s<CalculationType, Parameters>(par)
        {
            detail::aeqd::setup_aeqd(this->m_par, this->m_proj_parm, true, false);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Static projection
        template <typename BGP, typename CT, typename P>
        struct static_projection_type<srs::par4::aeqd, srs_sphere_tag, BGP, CT, P>
        {
            typedef aeqd_s<CT, P> type;
        };
        template <typename BGP, typename CT, typename P>
        struct static_projection_type<srs::par4::aeqd, srs_spheroid_tag, BGP, CT, P>
        {
            typedef aeqd_e_static<BGP, CT, P> type;
        };
        //BOOST_GEOMETRY_PROJECTIONS_DETAIL_STATIC_PROJECTION(srs::par4::aeqd, aeqd_s, aeqd_e_static)
        //BOOST_GEOMETRY_PROJECTIONS_DETAIL_STATIC_PROJECTION(srs::par4::aeqd_guam, aeqd_guam, aeqd_guam)

        // Factory entry(s)
        template <typename CalculationType, typename Parameters>
        class aeqd_entry : public detail::factory_entry<CalculationType, Parameters>
        {
            public :
                virtual base_v<CalculationType, Parameters>* create_new(const Parameters& par) const
                {
                    bool const guam = pj_param(par.params, "bguam").i != 0;

                    if (par.es && ! guam)
                        return new base_v_fi<aeqd_e<CalculationType, Parameters>, CalculationType, Parameters>(par);
                    else if (par.es && guam)
                        return new base_v_fi<aeqd_e_guam<CalculationType, Parameters>, CalculationType, Parameters>(par);
                    else
                        return new base_v_fi<aeqd_s<CalculationType, Parameters>, CalculationType, Parameters>(par);
                }
        };

        template <typename CalculationType, typename Parameters>
        inline void aeqd_init(detail::base_factory<CalculationType, Parameters>& factory)
        {
            factory.add_to_factory("aeqd", new aeqd_entry<CalculationType, Parameters>);
        }

    } // namespace detail
    #endif // doxygen

} // namespace projections

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_PROJECTIONS_AEQD_HPP

