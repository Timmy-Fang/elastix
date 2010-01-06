/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxVarianceOverLastDimensionMetric_H__
#define __elxVarianceOverLastDimensionMetric_H__

#include "elxIncludes.h"
#include "itkVarianceOverLastDimensionImageMetric.h"

#include "elxTimer.h"

namespace elastix
{
using namespace itk;

  /** \class VarianceOverLastDimensionMetric
   * \brief Compute the sum of variances over the slowest varying dimension in the moving image, based on AdvancedImageToImageMetric...
   *
   * This Class is templated over the type of the fixed and moving
   * images to be compared.
   *
   * This metric computes the sum of variances over the slowest varying dimension in
   * the moving image. The spatial positions of the moving image are established 
   * through a Transform. Pixel values are taken from the Moving image.
   *
   * This implementation is based on the AdvancedImageToImageMetric, which means that:
   * \li It uses the ImageSampler-framework
   * \li It makes use of the compact support of B-splines, in case of B-spline transforms.
   * \li Image derivatives are computed using either the B-spline interpolator's implementation
   * or by nearest neighbor interpolation of a precomputed central difference image.
   * \li A minimum number of samples that should map within the moving image (mask) can be specified.
   * 
   * \ingroup RegistrationMetrics
   * \ingroup Metrics
   */

  template <class TElastix >
    class VarianceOverLastDimensionMetric:
    public
      VarianceOverLastDimensionImageMetric<
        ITK_TYPENAME MetricBase<TElastix>::FixedImageType,
        ITK_TYPENAME MetricBase<TElastix>::MovingImageType >,
    public MetricBase<TElastix>
  {
  public:

    /** Standard ITK-stuff. */
    typedef VarianceOverLastDimensionMetric                             Self;
    typedef VarianceOverLastDimensionImageMetric<
      typename MetricBase<TElastix>::FixedImageType,
      typename MetricBase<TElastix>::MovingImageType >    Superclass1;
    typedef MetricBase<TElastix>                          Superclass2;
    typedef SmartPointer<Self>                            Pointer;
    typedef SmartPointer<const Self>                      ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro( Self );
    
    /** Run-time type information (and related methods). */
    itkTypeMacro( VarianceOverLastDimensionMetric, VarianceOverLastDimensionImageMetric );
    
    /** Name of this class.
     * Use this name in the parameter file to select this specific metric. \n
     * example: <tt>(Metric "VarianceOverLastDimensionMetric")</tt>\n
     */
    elxClassNameMacro( "VarianceOverLastDimensionMetric" );

    /** Typedefs from the superclass. */
    typedef typename 
      Superclass1::CoordinateRepresentationType              CoordinateRepresentationType;
    typedef typename Superclass1::MovingImageType            MovingImageType;
    typedef typename Superclass1::MovingImagePixelType       MovingImagePixelType;
    typedef typename Superclass1::MovingImageConstPointer    MovingImageConstPointer;
    typedef typename Superclass1::FixedImageType             FixedImageType;
    typedef typename Superclass1::FixedImageConstPointer     FixedImageConstPointer;
    typedef typename Superclass1::FixedImageRegionType       FixedImageRegionType;
    typedef typename Superclass1::TransformType              TransformType;
    typedef typename Superclass1::TransformPointer           TransformPointer;
    typedef typename Superclass1::InputPointType             InputPointType;
    typedef typename Superclass1::OutputPointType            OutputPointType;
    typedef typename Superclass1::TransformParametersType    TransformParametersType;
    typedef typename Superclass1::TransformJacobianType      TransformJacobianType;
    typedef typename Superclass1::InterpolatorType           InterpolatorType;
    typedef typename Superclass1::InterpolatorPointer        InterpolatorPointer;
    typedef typename Superclass1::RealType                   RealType;
    typedef typename Superclass1::GradientPixelType          GradientPixelType;
    typedef typename Superclass1::GradientImageType          GradientImageType;
    typedef typename Superclass1::GradientImagePointer       GradientImagePointer;
    typedef typename Superclass1::GradientImageFilterType    GradientImageFilterType;
    typedef typename Superclass1::GradientImageFilterPointer GradientImageFilterPointer;
    typedef typename Superclass1::FixedImageMaskType         FixedImageMaskType;
    typedef typename Superclass1::FixedImageMaskPointer      FixedImageMaskPointer;
    typedef typename Superclass1::MovingImageMaskType        MovingImageMaskType;
    typedef typename Superclass1::MovingImageMaskPointer     MovingImageMaskPointer;
    typedef typename Superclass1::MeasureType                MeasureType;
    typedef typename Superclass1::DerivativeType             DerivativeType;
    typedef typename Superclass1::ParametersType             ParametersType;
    typedef typename Superclass1::FixedImagePixelType        FixedImagePixelType;
    typedef typename Superclass1::MovingImageRegionType      MovingImageRegionType;
    typedef typename Superclass1::ImageSamplerType           ImageSamplerType;
    typedef typename Superclass1::ImageSamplerPointer        ImageSamplerPointer;
    typedef typename Superclass1::ImageSampleContainerType   ImageSampleContainerType;
    typedef typename 
      Superclass1::ImageSampleContainerPointer               ImageSampleContainerPointer;
    typedef typename Superclass1::FixedImageLimiterType      FixedImageLimiterType;
    typedef typename Superclass1::MovingImageLimiterType     MovingImageLimiterType;
    typedef typename
      Superclass1::FixedImageLimiterOutputType               FixedImageLimiterOutputType;
    typedef typename
      Superclass1::MovingImageLimiterOutputType              MovingImageLimiterOutputType;
    typedef typename
      Superclass1::MovingImageDerivativeScalesType           MovingImageDerivativeScalesType;
    
    /** The fixed image dimension. */
    itkStaticConstMacro( FixedImageDimension, unsigned int,
      FixedImageType::ImageDimension );

    /** The moving image dimension. */
    itkStaticConstMacro( MovingImageDimension, unsigned int,
      MovingImageType::ImageDimension );
    
    /** Typedef's inherited from Elastix. */
    typedef typename Superclass2::ElastixType               ElastixType;
    typedef typename Superclass2::ElastixPointer            ElastixPointer;
    typedef typename Superclass2::ConfigurationType         ConfigurationType;
    typedef typename Superclass2::ConfigurationPointer      ConfigurationPointer;
    typedef typename Superclass2::RegistrationType          RegistrationType;
    typedef typename Superclass2::RegistrationPointer       RegistrationPointer;
    typedef typename Superclass2::ITKBaseType               ITKBaseType;
      
    /** Typedef for timer. */
    typedef tmr::Timer          TimerType;
    /** Typedef for timer. */
    typedef TimerType::Pointer  TimerPointer;
  
    /** Sets up a timer to measure the initialisation time and
     * calls the Superclass' implementation.
     */
    virtual void Initialize(void) throw (ExceptionObject);

    /** 
     * Do some things before each resolution:
     * \li Set CheckNumberOfSamples setting
     * \li Set UseNormalization setting
     */
    virtual void BeforeEachResolution(void);

  protected:

    /** The constructor. */
    VarianceOverLastDimensionMetric(){};
    /** The destructor. */
    virtual ~VarianceOverLastDimensionMetric() {}

  private:

    /** The private constructor. */
    VarianceOverLastDimensionMetric( const Self& ); // purposely not implemented
    /** The private copy constructor. */
    void operator=( const Self& );              // purposely not implemented
    
  }; // end class VarianceOverLastDimensionMetric


} // end namespace elastix


#ifndef ITK_MANUAL_INSTANTIATION
#include "elxVarianceOverLastDimensionMetric.hxx"
#endif

#endif // end #ifndef __elxVarianceOverLastDimensionMetric_H__

