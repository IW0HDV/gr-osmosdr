/* -*- c++ -*- */
/*
 * Copyright 2013 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_PERSEUS_SOURCE_C_H
#define INCLUDED_PERSEUS_SOURCE_C_H

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <gnuradio/sync_block.h>

#include "source_iface.h"
#include <perseus-sdr.h>

class perseus_source_c;

typedef boost::shared_ptr<perseus_source_c> perseus_source_c_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of perseus_source_c.
 *
 * To avoid accidental use of raw pointers, perseus_source_c's
 * constructor is private.  make_perseus_source_c is the public
 * interface for creating new instances.
 */
perseus_source_c_sptr make_perseus_source_c (const std::string & args = "");

/*!
 * \brief Provides a stream of complex samples.
 * \ingroup block
 */
class perseus_source_c :
    public gr::sync_block,
    public source_iface
{
private:
  // The friend declaration allows make_perseus_source_c to
  // access the private constructor.

  friend perseus_source_c_sptr make_perseus_source_c (const std::string & args);

  perseus_source_c (const std::string & args);

public:
  ~perseus_source_c ();

  bool start();
  bool stop();

  int work( int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items );

  static std::vector< std::string > get_devices();

  size_t get_num_channels( void );

  osmosdr::meta_range_t get_sample_rates( void );
  double set_sample_rate( double rate );
  double get_sample_rate( void );

  osmosdr::freq_range_t get_freq_range( size_t chan = 0 );
  double set_center_freq( double freq, size_t chan = 0 );
  double get_center_freq( size_t chan = 0 );
  double set_freq_corr( double ppm, size_t chan = 0 );
  double get_freq_corr( size_t chan = 0 );

  std::vector<std::string> get_gain_names( size_t chan = 0 );
  osmosdr::gain_range_t get_gain_range( size_t chan = 0 );
  osmosdr::gain_range_t get_gain_range( const std::string & name, size_t chan = 0 );

  double set_gain( double gain, size_t chan = 0 ) {return gain; }
  double get_gain( size_t chan = 0 ) {return _att_value; }

  double set_attenuator( double gain, size_t chan = 0 );
  double get_attenuator( size_t chan = 0 );

  double set_adc_preamp( double gain, size_t chan = 0 );
  double get_adc_preamp( size_t chan = 0 );

  double set_adc_dither( double gain, size_t chan = 0 );
  double get_adc_dither( size_t chan = 0 );

  double set_gain( double gain, const std::string & name, size_t chan = 0 );
  double get_gain( const std::string & name, size_t chan = 0 );

  std::vector< std::string > get_antennas( size_t chan = 0 );
  std::string set_antenna( const std::string & antenna, size_t chan = 0 );
  std::string get_antenna( size_t chan = 0 );


private:
  static int _perseus_rx_callback(void *buf, int buf_size, void *extra);

  int perseus_rx_callback(void *samples, int sample_count);

  perseus_descr *_dev;

  boost::circular_buffer<gr_complex> *_fifo;
  boost::mutex _fifo_lock;
  boost::condition_variable _samp_avail;

  std::vector< std::pair<double, uint32_t> > _sample_rates;
  double _sample_rate;
  double _center_freq;
  double _freq_corr;
  
  bool _frun;
  int _cnt;
  double _att_value;
  bool _adc_preamp;
  bool _adc_dither;
  bool _preselector;
};

#endif /* INCLUDED_PERSEUS_SOURCE_C_H */
