/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

/** Parts of the code were taken from an ITK file.
 * Original ITK copyright message, just for reference: */
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date: 2008-04-15 19:54:41 +0200 (Tue, 15 Apr 2008) $
  Version:   $Revision: 1573 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiResolutionGaussianSmoothingPyramidImageFilter_h
#define __itkMultiResolutionGaussianSmoothingPyramidImageFilter_h

#include "itkMultiResolutionPyramidImageFilter.h"

namespace itk
{

/** \class MultiResolutionGaussianSmoothingPyramidImageFilter
 * \brief Framework for creating images in a multi-resolution
 * pyramid.
 *
 * MultiResolutionGaussianSmoothingPyramidImageFilter creates
 * an image pryamid according to a user defined multi-resolution schedule.
 * 
 * This class inherits from the MultiResolutionPyramidImageFilter. It
 * applies the same smoothing but does NOT do the downsampling.
 * 
 * The multi-resolution schedule is still specified in terms for
 * 'shrink factors' at each multi-resolution level for each dimension
 * (although, actual shrinking is not performed).
 * 
 * A user can either use the default schedules or specify 
 * each factor in the schedules directly.
 *
 * The schedule is stored as an unsigned int matrix.
 * An element of the table can be access via the double bracket
 * notation: table[resLevel][dimension]
 *
 * For example:
 *   8 4 4
 *   4 4 2
 *
 * is a schedule for two computation level. In the first (coarest)
 * level the image is reduce by a factor of 8 in the column dimension,
 * factor of 4 in the row dimension and factor of 4 in the slice dimension.
 * In the second level, the image is reduce by a factor of 4 in the column
 * dimension, 4 is the row dimension and 2 in the slice dimension.
 * 
 * The method SetNumberOfLevels() set the number of
 * computation levels in the pyramid. This method will
 * allocate memory for the multi-resolution schedule table.
 * This method generates defaults tables with the starting
 * shrink factor for all dimension set to  2^(NumberOfLevel - 1). 
 * All factors are halved for all subsequent levels. 
 * For example if the number of levels was set to 4, the default table is:
 *
 * 8 8 8
 * 4 4 4
 * 2 2 2
 * 1 1 1
 *
 * The user can get a copy of the schedule via GetSchedule()
 * They may make alteration and reset it using SetSchedule().
 *
 * A user can create a default table by specifying the starting
 * shrink factors via methods SetStartingShrinkFactors()
 * The factors for subsequent level is generated by 
 * halving the factor or setting to one, depending on which is larger.
 *
 * For example, for 4 levels and starting factors of 8,8,4
 * the default table would be:
 *
 * 8 8 4
 * 4 4 2
 * 2 2 1
 * 1 1 1
 *
 * When this filter is updated, NumberOfLevels outputs are produced.
 * The N'th output correspond to the N'th level of the pyramid.
 * 
 * To generate each output image, Gaussian smoothing is first performed 
 * using a series of RecursiveGaussianImageFilter with standard deviation
 * (shrink factor / 2)*imagespacing. 
 * The smoothed images are NOT downsampled, in contrast to the superclass's
 * behaviour.
 *
 * This class is templated over the input image type and the output image 
 * type.
 *
 * This filter uses multithreaded filters to perform the smoothing.
 * 
 * This filter supports streaming.
 *
 * \ingroup PyramidImageFilter Multithreaded Streamed
 */
template <
  class TInputImage, 
  class TOutputImage
  >
class MultiResolutionGaussianSmoothingPyramidImageFilter : 
    public MultiResolutionPyramidImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef MultiResolutionGaussianSmoothingPyramidImageFilter  Self;
  typedef MultiResolutionPyramidImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiResolutionGaussianSmoothingPyramidImageFilter, MultiResolutionPyramidImageFilter);

  /** ImageDimension enumeration. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Inherit types from Superclass. */
  typedef typename Superclass::ScheduleType ScheduleType;
  typedef typename Superclass::InputImageType InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
  typedef typename Superclass::InputImagePointer InputImagePointer;
  typedef typename Superclass::OutputImagePointer OutputImagePointer;
  typedef typename Superclass::InputImageConstPointer InputImageConstPointer;
  
  /** Set a multi-resolution schedule.  The input schedule must have only
   * ImageDimension number of columns and NumberOfLevels number of rows. In 
   * contrast to the superclass, any schedule is allowed:
   * - For each dimension, the shrink factor may be non-increasing with respect to
   * subsequent levels. 
   * - shrink factors of 0 are allowed. This results in almost no smoothing. 
   * Because of lazy programming, the image is then smoothed with a gauss with sigma
   * of 0.01*spacing...
   *
   * Note that the images are not actually shrunk by this class. They are 
   * only smoothed with the same standard deviation gaussian as used by
   * the superclass.
   */
  void SetSchedule( const ScheduleType& schedule );

  /** Set spacing etc. */
  virtual void GenerateOutputInformation();

  /** Given one output whose requested region has been set, this method sets
   * the requested region for the remaining output images.  The original
   * documentation of this method is below.  \sa
   * ProcessObject::GenerateOutputRequestedRegion(); */
  virtual void GenerateOutputRequestedRegion(DataObject *output);

  /** MultiResolutionGaussianSmoothingPyramidImageFilter requires a larger input requested
   * region than the output requested regions to accomdate the 
   * smoothing operations. As such, MultiResolutionGaussianSmoothingPyramidImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion().  The
   * original documentation of this method is below.  \sa
   * ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();
 
protected:
  MultiResolutionGaussianSmoothingPyramidImageFilter();
  ~MultiResolutionGaussianSmoothingPyramidImageFilter() {};
  void PrintSelf(std::ostream&os, Indent indent) const;

  /** Generate the output data. */
  void GenerateData();

  /** This filter by default generates the largest possible region,
   * because it uses internally a filter that does this. */
  virtual void EnlargeOutputRequestedRegion(DataObject *output);


private:
  MultiResolutionGaussianSmoothingPyramidImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiResolutionGaussianSmoothingPyramidImageFilter.txx"
#endif

#endif


