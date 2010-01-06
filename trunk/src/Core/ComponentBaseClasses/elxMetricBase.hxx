/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxMetricBase_hxx
#define __elxMetricBase_hxx

#include "elxMetricBase.h"

namespace elastix
{
using namespace itk;

/**
 * ********************* Constructor ****************************
 */

template <class TElastix>
MetricBase<TElastix>
::MetricBase()
{
  /** Initialize. */
  this->m_ShowExactMetricValue = false;
  this->m_ExactMetricSampler = 0;

} // end Constructor


/**
 * ******************* BeforeEachResolutionBase ******************
 */

template <class TElastix>
void
MetricBase<TElastix>
::BeforeEachResolutionBase( void )
{
  /** Get the current resolution level. */
  unsigned int level
    = this->m_Registration->GetAsITKBaseType()->GetCurrentLevel();

  /** Check if the exact metric value, computed on all pixels, should be shown, 
   * and whether the all pixels should be used during optimisation.
   */

  /** Define the name of the ExactMetric column */
  std::string exactMetricColumn = "Exact";
  exactMetricColumn += this->GetComponentLabel();

  /** Remove the ExactMetric-column, if it already existed. */
  xl::xout["iteration"].RemoveTargetCell( exactMetricColumn.c_str() );

  /** Read the parameter file: Show the exact metric in every iteration? */ 
  bool showExactMetricValue = false;
  this->GetConfiguration()->ReadParameter( showExactMetricValue,
    "ShowExactMetricValue", this->GetComponentLabel(), level, 0 );
  this->m_ShowExactMetricValue = showExactMetricValue;
  if ( showExactMetricValue )
  {
    /** Create a new column in the iteration info table */
    xl::xout["iteration"].AddTargetCell( exactMetricColumn.c_str() );
    xl::xout["iteration"][ exactMetricColumn.c_str() ]
      << std::showpoint << std::fixed;
  }

  /** Cast this to AdvancedMetricType. */
  AdvancedMetricType * thisAsAdvanced
    = dynamic_cast< AdvancedMetricType * >( this );

  /** For advanced metrics several other things can be set. */
  if ( thisAsAdvanced != 0 )
  {
    /** Should the metric check for enough samples? */
    bool checkNumberOfSamples = true;
    this->GetConfiguration()->ReadParameter( checkNumberOfSamples, 
      "CheckNumberOfSamples", this->GetComponentLabel(), level, 0 );

    /** Get the required ratio. */
    float ratio = 0.25;
    this->GetConfiguration()->ReadParameter( ratio,
      "RequiredRatioOfValidSamples", this->GetComponentLabel(), level, 0, false );

    /** Set it. */
    if ( !checkNumberOfSamples )
    {
      thisAsAdvanced->SetRequiredRatioOfValidSamples( 0.0 );
    }
    else
    {
      thisAsAdvanced->SetRequiredRatioOfValidSamples( ratio );
    }
  } // end Advanced metric

} // end BeforeEachResolutionBase()


/**
 * ******************* AfterEachIterationBase ******************
 */

template <class TElastix>
void
MetricBase<TElastix>
::AfterEachIterationBase( void )
{ 
  /** Show the metric value computed on all voxels, if the user wanted it. */

  /** Define the name of the ExactMetric column (ExactMetric<i>). */
  std::string exactMetricColumn = "Exact";
  exactMetricColumn += this->GetComponentLabel();

  if ( this->m_ShowExactMetricValue )
  {
    xl::xout["iteration"][ exactMetricColumn.c_str() ]
      << this->GetExactValue( this->GetElastix()->GetElxOptimizerBase()
      ->GetAsITKBaseType()->GetCurrentPosition() );
  }

} // end AfterEachIterationBase()


/**
 * ********************* SelectNewSamples ************************
 */

template <class TElastix>
void
MetricBase<TElastix>
::SelectNewSamples( void )
{
  if ( this->GetAdvancedMetricImageSampler() )
  {
    /** Force the metric to base its computation on a new subset of image samples. */
    this->GetAdvancedMetricImageSampler()->SelectNewSamplesOnUpdate();
  }
  else
  {
    /** Not every metric may have implemented this, so give a warning when this
     * method is called for a metric without sampler support.
     * To avoid the warning, this method may be overridden by a subclass.
     */
    xl::xout["warning"] 
      << "WARNING: The NewSamplesEveryIteration option was set to \"true\", but "
      << this->GetComponentLabel()
      << " does not use a sampler."
      << std::endl;
  }

} // end SelectNewSamples()


/**
 * ********************* GetExactValue ************************
 */

template <class TElastix>
typename MetricBase<TElastix>::MeasureType
MetricBase<TElastix>
::GetExactValue( const ParametersType& parameters )
{ 
  /** Get the current image sampler. */
  typename ImageSamplerBaseType::Pointer currentSampler
    = this->GetAdvancedMetricImageSampler();

  /** Useless implementation if no image sampler is used; we may as
   * well throw an error, but the ShowExactMetricValue is not really
   * essential for good registration...
   */
  if ( currentSampler.IsNull() )
  {      
    return itk::NumericTraits<MeasureType>::Zero;
  }

  /** Try to cast the current Sampler to a FullSampler. */
  ImageFullSamplerType * testPointer
    = dynamic_cast<ImageFullSamplerType *>( currentSampler.GetPointer() );
  if ( testPointer != 0 )
  {
    /** GetValue gives us the exact value! */
    return this->GetAsITKBaseType()->GetValue(parameters);
  }

  /** We have to provide the metric a full sampler, calls its GetValue
   * and set back its original sampler.
   */
  if ( this->m_ExactMetricSampler.IsNull() )
  {
    this->m_ExactMetricSampler = ImageFullSamplerType::New();
  }

  /** Copy settings from current sampler. */
  this->m_ExactMetricSampler->SetInput( currentSampler->GetInput() );
  this->m_ExactMetricSampler->SetMask( currentSampler->GetMask() );      
  this->m_ExactMetricSampler->SetInputImageRegion(
    currentSampler->GetInputImageRegion() );
  this->SetAdvancedMetricImageSampler( this->m_ExactMetricSampler );

  /** Compute the metric value on the full images. */
  MeasureType exactValue
    = this->GetAsITKBaseType()->GetValue( parameters );

  /** Reset the original sampler. */
  this->SetAdvancedMetricImageSampler( currentSampler );

  return exactValue;

} // end GetExactValue()


/**
 * ******************* GetAdvancedMetricUseImageSampler ********************
 */

template <class TElastix>
bool
MetricBase<TElastix>
::GetAdvancedMetricUseImageSampler( void ) const
{
  /** Cast this to AdvancedMetricType. */
  const AdvancedMetricType * thisAsMetricWithSampler
    = dynamic_cast< const AdvancedMetricType * >( this );

  /** If no AdvancedMetricType, return false. */
  if ( thisAsMetricWithSampler == 0 )
  {
    return false;
  }

  return thisAsMetricWithSampler->GetUseImageSampler();

} // end GetAdvancedMetricUseImageSampler()


/**
 * ******************* SetAdvancedMetricImageSampler ********************
 */

template <class TElastix>
void MetricBase<TElastix>
::SetAdvancedMetricImageSampler( ImageSamplerBaseType * sampler )
{
  /** Cast this to AdvancedMetricType. */
  AdvancedMetricType * thisAsMetricWithSampler
    = dynamic_cast< AdvancedMetricType * >( this );

  /** If no AdvancedMetricType, or if the MetricWithSampler does not
   * utilize the sampler, return.
   */
  if ( thisAsMetricWithSampler == 0 )
  {
    return;
  }
  if ( thisAsMetricWithSampler->GetUseImageSampler() == false )
  {
    return;
  }

  /** Set the sampler. */
  thisAsMetricWithSampler->SetImageSampler( sampler );

} // end SetAdvancedMetricImageSampler()


/**
 * ******************* GetAdvancedMetricImageSampler ********************
 */

template <class TElastix>
typename MetricBase<TElastix>::ImageSamplerBaseType *
MetricBase<TElastix>
::GetAdvancedMetricImageSampler( void ) const
{
  /** Cast this to AdvancedMetricType. */
  const AdvancedMetricType * thisAsMetricWithSampler
    = dynamic_cast< const AdvancedMetricType * >( this );

  /** If no AdvancedMetricType, or if the MetricWithSampler does not
   * utilize the sampler, return 0.
   */
  if ( thisAsMetricWithSampler == 0 )
  {
    return 0;
  }
  if ( thisAsMetricWithSampler->GetUseImageSampler() == false )
  {
    return 0;
  }

  /** Return the sampler. */
  return thisAsMetricWithSampler->GetImageSampler();

} // end GetAdvancedMetricImageSampler()


} // end namespace elastix


#endif // end #ifndef __elxMetricBase_hxx

