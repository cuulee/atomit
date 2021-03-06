/**
 * Atom-IT - A Lightweight, RESTful microservice for IoT
 * Copyright (C) 2017 Sebastien Jodogne, WSL S.A., Belgium
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * In addition, as a special exception, the copyright holders of this
 * program give permission to link the code of its release with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables. You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL". If you
 * modify file(s) with this exception, you may extend this exception to
 * your version of the file(s), but you are not obligated to do so. If
 * you do not wish to do so, delete this exception statement from your
 * version. If you delete this exception statement from all source files
 * in the program, then also delete it here.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/


#include "HttpPostSinkFilter.h"

#include <Core/Toolbox.h>

namespace AtomIT
{
  AdapterFilter::PushStatus HttpPostSinkFilter::Push(const Message& message)
  {
    std::string mime = "application/octet-stream";

    if (Orthanc::Toolbox::IsAsciiString(mime.c_str(), mime.size()))
    {
      std::vector<std::string> tokens;
      Orthanc::Toolbox::TokenizeString(tokens, message.GetMetadata(), '/');
      if (tokens.size() == 2)
      {
        mime = message.GetMetadata();
      }
    }

    client_.ClearHeaders();
    client_.AddHeader("Content-Type", mime);
    client_.SetBody(message.GetValue());

    std::string answer;
    if (client_.Apply(answer))
    {
      return PushStatus_Success;
    }
    else
    {
      return PushStatus_Failure;
    }
  }


  HttpPostSinkFilter::HttpPostSinkFilter(const std::string& name,
                                         ITimeSeriesManager& manager,
                                         const std::string& timeSeries,
                                         const std::string& url) :
    AdapterFilter(name, manager, timeSeries)
  {
    client_.SetMethod(Orthanc::HttpMethod_Post);
    client_.SetRedirectionFollowed(true);
    client_.SetUrl(url);
  }
}
