!
! CDDL HEADER START
!
! The contents of this file are subject to the terms of the Common Development
! and Distribution License Version 1.0 (the "License").
!
! You can obtain a copy of the license at
! http://www.opensource.org/licenses/CDDL-1.0.  See the License for the
! specific language governing permissions and limitations under the License.
!
! When distributing Covered Code, include this CDDL HEADER in each file and
! include the License file in a prominent location with the name LICENSE.CDDL.
! If applicable, add the following below this CDDL HEADER, with the fields
! enclosed by brackets "[]" replaced with your own identifying information:
!
! Portions Copyright (c) [yyyy] [name of copyright owner]. All rights reserved.
!
! CDDL HEADER END
!

!
! Copyright (c) 2016--2021, Regents of the University of Minnesota.
! All rights reserved.
!
! Contributors:
!    Ryan S. Elliott
!

!
! Release: This file is part of the kim-api.git repository.
!

!> \brief \copybrief KIM::Numbering
!!
!! \sa KIM::Numbering, KIM_Numbering
!!
!! \since 2.0
module kim_numbering_module
  use, intrinsic :: iso_c_binding
  implicit none
  private

  public &
    ! Derived types
    kim_numbering_type, &
    ! Constants
    KIM_NUMBERING_ZERO_BASED, &
    KIM_NUMBERING_ONE_BASED, &
    ! Routines
    kim_known, &
    operator(.eq.), &
    operator(.ne.), &
    kim_from_string, &
    kim_to_string, &
    kim_get_number_of_numberings, &
    kim_get_numbering

  !> \brief \copybrief KIM::Numbering
  !!
  !! \sa KIM::Numbering, KIM_Numbering
  !!
  !! \since 2.0
  type, bind(c) :: kim_numbering_type
    integer(c_int) :: numbering_id
  end type kim_numbering_type

  !> \brief \copybrief KIM::NUMBERING::zeroBased
  !!
  !! \sa KIM::NUMBERING::zeroBased, KIM_NUMBERING_zeroBased
  !!
  !! \since 2.0
  type(kim_numbering_type), protected, save, &
    bind(c, name="KIM_NUMBERING_zeroBased") &
    :: KIM_NUMBERING_ZERO_BASED

  !> \brief \copybrief KIM::NUMBERING::oneBased
  !!
  !! \sa KIM::NUMBERING::oneBased, KIM_NUMBERING_oneBased
  !!
  !! \since 2.0
  type(kim_numbering_type), protected, save, &
    bind(c, name="KIM_NUMBERING_oneBased") &
    :: KIM_NUMBERING_ONE_BASED

  !> \brief \copybrief KIM::Numbering::Known
  !!
  !! \sa KIM::Numbering::Known, KIM_Numbering_Known
  !!
  !! \since 2.0
  interface kim_known
    module procedure kim_numbering_known
  end interface kim_known

  !> \brief \copybrief KIM::Numbering::operator==()
  !!
  !! \sa KIM::Numbering::operator==(), KIM_Numbering_Equal
  !!
  !! \since 2.0
  interface operator(.eq.)
    module procedure kim_numbering_equal
  end interface operator(.eq.)

  !> \brief \copybrief KIM::Numbering::operator!=()
  !!
  !! \sa KIM::Numbering::operator!=(), KIM_Numbering_NotEqual
  !!
  !! \since 2.0
  interface operator(.ne.)
    module procedure kim_numbering_not_equal
  end interface operator(.ne.)

  !> \brief \copybrief KIM::Numbering::Numbering(std::string const &)
  !!
  !! \sa KIM::Numbering::Numbering(std::string const &),
  !! KIM_Numbering_FromString
  !!
  !! \since 2.0
  interface kim_from_string
    module procedure kim_numbering_from_string
  end interface kim_from_string

  !> \brief \copybrief KIM::Numbering::ToString
  !!
  !! \sa KIM::Numbering::ToString, KIM_Numbering_ToString
  !!
  !! \since 2.0
  interface kim_to_string
    module procedure kim_numbering_to_string
  end interface kim_to_string

contains
  !> \brief \copybrief KIM::Numbering::Known
  !!
  !! \sa KIM::Numbering::Known, KIM_Numbering_Known
  !!
  !! \since 2.0
  logical recursive function kim_numbering_known(numbering)
    implicit none
    interface
      integer(c_int) recursive function known(numbering) &
        bind(c, name="KIM_Numbering_Known")
        use, intrinsic :: iso_c_binding
        import kim_numbering_type
        implicit none
        type(kim_numbering_type), intent(in), value :: numbering
      end function known
    end interface
    type(kim_numbering_type), intent(in) :: numbering

    kim_numbering_known = (known(numbering) /= 0)
  end function kim_numbering_known

  !> \brief \copybrief KIM::Numbering::operator==()
  !!
  !! \sa KIM::Numbering::operator==(), KIM_Numbering_Equal
  !!
  !! \since 2.0
  logical recursive function kim_numbering_equal(lhs, rhs)
    implicit none
    type(kim_numbering_type), intent(in) :: lhs
    type(kim_numbering_type), intent(in) :: rhs

    kim_numbering_equal = (lhs%numbering_id == rhs%numbering_id)
  end function kim_numbering_equal

  !> \brief \copybrief KIM::Numbering::operator!=()
  !!
  !! \sa KIM::Numbering::operator!=(), KIM_Numbering_NotEqual
  !!
  !! \since 2.0
  logical recursive function kim_numbering_not_equal(lhs, rhs)
    implicit none
    type(kim_numbering_type), intent(in) :: lhs
    type(kim_numbering_type), intent(in) :: rhs

    kim_numbering_not_equal = .not. (lhs == rhs)
  end function kim_numbering_not_equal

  !> \brief \copybrief KIM::Numbering::Numbering(std::string const &)
  !!
  !! \sa KIM::Numbering::Numbering(std::string const &),
  !! KIM_Numbering_FromString
  !!
  !! \since 2.0
  recursive subroutine kim_numbering_from_string(string, numbering)
    implicit none
    interface
      type(kim_numbering_type) recursive function from_string(string) &
        bind(c, name="KIM_Numbering_FromString")
        use, intrinsic :: iso_c_binding
        import kim_numbering_type
        implicit none
        character(c_char), intent(in) :: string(*)
      end function from_string
    end interface
    character(len=*, kind=c_char), intent(in) :: string
    type(kim_numbering_type), intent(out) :: numbering

    numbering = from_string(trim(string)//c_null_char)
  end subroutine kim_numbering_from_string

  !> \brief \copybrief KIM::Numbering::ToString
  !!
  !! \sa KIM::Numbering::ToString, KIM_Numbering_ToString
  !!
  !! \since 2.0
  recursive subroutine kim_numbering_to_string(numbering, string)
    use kim_convert_string_module, only: kim_convert_c_char_ptr_to_string
    implicit none
    interface
      type(c_ptr) recursive function get_string(numbering) &
        bind(c, name="KIM_Numbering_ToString")
        use, intrinsic :: iso_c_binding
        import kim_numbering_type
        implicit none
        type(kim_numbering_type), intent(in), value :: numbering
      end function get_string
    end interface
    type(kim_numbering_type), intent(in) :: numbering
    character(len=*, kind=c_char), intent(out) :: string

    type(c_ptr) :: p

    p = get_string(numbering)
    call kim_convert_c_char_ptr_to_string(p, string)
  end subroutine kim_numbering_to_string

  !> \brief \copybrief KIM::NUMBERING::GetNumberOfNumberings
  !!
  !! \sa KIM::NUMBERING::GetNumberOfNumberings,
  !! KIM_NUMBERING_GetNumberOfNumberings
  !!
  !! \since 2.0
  recursive subroutine kim_get_number_of_numberings(number_of_numberings)
    implicit none
    interface
      recursive subroutine get_number_of_numberings(number_of_numberings) &
        bind(c, name="KIM_NUMBERING_GetNumberOfNumberings")
        use, intrinsic :: iso_c_binding
        implicit none
        integer(c_int), intent(out) :: number_of_numberings
      end subroutine get_number_of_numberings
    end interface
    integer(c_int), intent(out) :: number_of_numberings

    call get_number_of_numberings(number_of_numberings)
  end subroutine kim_get_number_of_numberings

  !> \brief \copybrief KIM::NUMBERING::GetNumbering
  !!
  !! \sa KIM::NUMBERING::GetNumbering, KIM_NUMBERING_GetNumbering
  !!
  !! \since 2.0
  recursive subroutine kim_get_numbering(index, numbering, ierr)
    implicit none
    interface
      integer(c_int) recursive function get_numbering(index, numbering) &
        bind(c, name="KIM_NUMBERING_GetNumbering")
        use, intrinsic :: iso_c_binding
        import kim_numbering_type
        implicit none
        integer(c_int), intent(in), value :: index
        type(kim_numbering_type), intent(out) :: numbering
      end function get_numbering
    end interface
    integer(c_int), intent(in) :: index
    type(kim_numbering_type), intent(out) :: numbering
    integer(c_int), intent(out) :: ierr

    ierr = get_numbering(index - 1, numbering)
  end subroutine kim_get_numbering
end module kim_numbering_module
