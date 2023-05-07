// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#include "DrmDisplay.hpp"
#include "Hardware/DisplayDPI.hpp"
#include "system/Error.hxx"

#include <span>

#include <fcntl.h>

namespace EGL {

//static UniqueFileDescriptor
//OpenDriDevice(const char *path)
//{
//  UniqueFileDescriptor fd;
//  if (!fd.Open(path, O_RDWR))
//    throw FormatErrno("Could not open DRI device %s", path);
//
//  /* check if this card works */
//  drmModeRes *resources = drmModeGetResources(fd.Get());
//  if (resources == nullptr)
//    throw FormatErrno("drmModeGetResources() for DRI device %s failed", path);
//
//  return fd;
//}
//
//
//
//static auto OpenDriDevice()
//{
//    if (const char *dri_device = getenv("DRI_DEVICE"))
//    {
//        return OpenDriDevice(dri_device);
//    }
//
//  /* some computers like the Raspberry Pi and the CubieBoard have two
//     cards in /dev/dri/, of which only one works, but it's hard to
//     tell which one; probe both */
//  try
//  {
//    return OpenDriDevice("/dev/dri/card0");
//  } catch (...) {
//    try {
//      return OpenDriDevice("/dev/dri/card1");
//    } catch (...) {
//    }
//
//    /* if the second one fails, report the error for the first one */
//    throw;
//  }
//}

UniqueFileDescriptor DrmDisplay::dri_fd2;

void DrmDisplay::DisplayOpenDriDevice()
{
    const char path0[] = "/dev/dri/card0";
    const char path1[] = "/dev/dri/card1";

    if (const char *dri_device = getenv("DRI_DEVICE"))
	{
        dri_fd2.Open(dri_device, O_RDWR);
	}
    else
    {
		try
		{

			if (!dri_fd2.Open(path0, O_RDWR))
			{
				throw FormatErrno("Could not open DRI device %s", path0);
			}
		}
		catch (...)
		{
			try
			{
				if (!dri_fd2.Open(path1, O_RDWR))
				{
					throw FormatErrno("Could not open DRI device %s", path1);
				}
			}
			catch (...)
			{

			}
		}
    }

    try
    {

		drmModeRes *resources = drmModeGetResources(dri_fd2.Get());
		  if (resources == nullptr)
			throw FormatErrno("drmModeGetResources() for DRI device failed");
		  printf("fd 2 %i \n", dri_fd2.Get());
    }
    catch (...)
	{

	}
}

FileDescriptor DrmDisplay::getDisplayOpenDriDevice()
{
    return dri_fd2;
}
//static drmModeConnector *
//ChooseConnector(FileDescriptor dri_fd,
//                const std::span<const uint32_t> connectors)
//{
//  for (const auto id : connectors) {
//    auto *connector = drmModeGetConnector(dri_fd.Get(), id);
//    if (connector != nullptr && connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0)
//    {
//      return connector;
//    }
//    drmModeFreeConnector(connector);
//  }
//
//  throw std::runtime_error("No usable DRM connector found");
//}

void DrmDisplay::findConnector(const drmModeRes* resources)
{
    drmModeConnector *connector = NULL;
    for (uint8_t i = 0; i < resources->count_connectors; i++) {
        connector = drmModeGetConnector(dri_fd2.Get(), resources->connectors[i]);
        if (connector->connection == DRM_MODE_CONNECTED)
        {
            this->m_connectors[this->m_count_connectors] = connector;
            this->m_count_connectors++;

        }
        else
        {
          drmModeFreeConnector(connector);
        }
    }
}

DrmDisplay::DrmDisplay(uint8_t connectorPort)
{
  //printf("fd 1 %i \n", dri_fd.Get());
  drmModeRes *resources = drmModeGetResources(dri_fd2.Get());
  if (resources == nullptr)
    throw MakeErrno("drmModeGetResources() failed");

  findConnector(resources);
  auto *connector = this->m_connectors[connectorPort];
  connector_id = connector->connector_id;

  if (auto *encoder = drmModeGetEncoder(dri_fd2.Get(), connector->encoder_id)) {
    crtc_id = encoder->crtc_id;
    drmModeFreeEncoder(encoder);
  } else
    throw std::runtime_error("No usable DRM encoder found");

  mode = connector->modes[0];

  size_mm = {connector->mmWidth, connector->mmHeight};

  drmModeFreeConnector(connector);
}

DrmDisplay::~DrmDisplay() noexcept = default;

//void DrmDisplay::switchConnector(uint8_t connectorPort)
//{
//    drmModeRes *resources = drmModeGetResources(dri_fd.Get());
//    if (resources == nullptr)
//      throw MakeErrno("drmModeGetResources() failed");
//
//    auto *connector = this->m_connectors[connectorPort];
//    connector_id = connector->connector_id;
//
//    if (auto *encoder = drmModeGetEncoder(dri_fd.Get(), connector->encoder_id)) {
//      crtc_id = encoder->crtc_id;
//      drmModeFreeEncoder(encoder);
//    } else
//      throw std::runtime_error("No usable DRM encoder found");
//
//    mode = connector->modes[0];
//
//    size_mm = {connector->mmWidth, connector->mmHeight};
//
//    drmModeFreeConnector(connector);
//}

void
DrmDisplay::SetMaster()
{
  if (drmSetMaster(dri_fd2.Get()) != 0)
    throw MakeErrno("DRM_IOCTL_SET_MASTER failed");
}

void
DrmDisplay::DropMaster() noexcept
{
  drmDropMaster(dri_fd2.Get());
}

} // namespace EGL
