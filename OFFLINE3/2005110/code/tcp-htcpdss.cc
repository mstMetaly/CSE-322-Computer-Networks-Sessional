/*
 * Copyright (c) 2015 ResiliNets, ITTC, University of Kansas
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * by: Amir Modarresi <amodarresi@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  https://resilinets.org/
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 */

#include "tcp-htcpdss.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TcpHtcpDSS");

NS_OBJECT_ENSURE_REGISTERED(TcpHtcpDSS);

TypeId
TcpHtcpDSS::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpHtcpDSS")
                            .SetParent<TcpNewReno>()
                            .AddConstructor<TcpHtcpDSS>()
                            .SetGroupName("Internet")
                            .AddAttribute("DefaultBackoff",
                                          "The default AIMD backoff factor",
                                          DoubleValue(0.5),
                                          MakeDoubleAccessor(&TcpHtcpDSS::m_defaultBackoff),
                                          MakeDoubleChecker<double>(0, 1))
                            .AddAttribute("ThroughputRatio",
                                          "Threshold value for updating beta",
                                          DoubleValue(0.2),
                                          MakeDoubleAccessor(&TcpHtcpDSS::m_throughputRatio),
                                          MakeDoubleChecker<double>())
                            .AddAttribute("DeltaL",
                                          "Delta_L parameter in increase function",
                                          TimeValue(Seconds(1)),
                                          MakeTimeAccessor(&TcpHtcpDSS::m_deltaL),
                                          MakeTimeChecker())
                            .AddAttribute("RTTThresholdHigh",
                                          "High RTT threshold for fast window growth",
                                          DoubleValue(0.05),
                                          MakeDoubleAccessor(&TcpHtcpDSS::m_rttThresholdHigh),
                                          MakeDoubleChecker<double>())
                            .AddAttribute("RTTThresholdLow",
                                          "Low RTT threshold for moderate window growth",
                                          DoubleValue(0.2),
                                          MakeDoubleAccessor(&TcpHtcpDSS::m_rttThresholdLow),
                                          MakeDoubleChecker<double>());
    return tid;
}

std::string
TcpHtcpDSS::GetName() const
{
    return "TcpHtcpDSS";
}

TcpHtcpDSS::TcpHtcpDSS()
    : TcpNewReno(),
      m_alpha(0),
      m_beta(0),
      m_delta(0),
      m_lastCon(0),
      m_minRtt(Time::Max()),
      m_maxRtt(Time::Min()),
      m_throughput(0),
      m_lastThroughput(0),
      m_dataSent(0),
      m_rttThresholdHigh(0.05),  // Set your high RTT threshold here (for example, 0.05)
      m_rttThresholdLow(0.2)     // Set your low RTT threshold here (for example, 0.2)
{
    NS_LOG_FUNCTION(this);
}

TcpHtcpDSS::TcpHtcpDSS(const TcpHtcpDSS& sock)
    : TcpNewReno(sock),
      m_alpha(sock.m_alpha),
      m_beta(sock.m_beta),
      m_defaultBackoff(sock.m_defaultBackoff),
      m_throughputRatio(sock.m_throughputRatio),
      m_delta(sock.m_delta),
      m_deltaL(sock.m_deltaL),
      m_lastCon(sock.m_lastCon),
      m_minRtt(sock.m_minRtt),
      m_maxRtt(sock.m_maxRtt),
      m_throughput(sock.m_throughput),
      m_lastThroughput(sock.m_lastThroughput),
      m_dataSent(sock.m_dataSent),
      m_rttThresholdHigh(sock.m_rttThresholdHigh),
      m_rttThresholdLow(sock.m_rttThresholdLow)
{
    NS_LOG_FUNCTION(this);
}

TcpHtcpDSS::~TcpHtcpDSS()
{
    NS_LOG_FUNCTION(this);
}

Ptr<TcpCongestionOps>
TcpHtcpDSS::Fork()
{
    NS_LOG_FUNCTION(this);
    return CopyObject<TcpHtcpDSS>(this);
}

void
TcpHtcpDSS::CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);
    if (segmentsAcked > 0)
    {
        double adder = static_cast<double>( 
            ((tcb->m_segmentSize * tcb->m_segmentSize) + (tcb->m_cWnd * m_alpha)) / tcb->m_cWnd);
        adder = std::max(1.0, adder);
        tcb->m_cWnd += static_cast<uint32_t>(adder);
        NS_LOG_INFO("In CongAvoid, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                     << tcb->m_ssThresh);
    }
}

void
TcpHtcpDSS::UpdateAlpha()
{
    NS_LOG_FUNCTION(this);

    m_delta = (Simulator::Now() - m_lastCon);
    if (m_delta <= m_deltaL)
    {
        m_alpha = 1;
    }
    else
    {
        Time diff = m_delta - m_deltaL;
        double diffSec = diff.GetSeconds();
        // alpha=1+10(Delta-Delta_L)+[0.5(Delta-Delta_L)]^2  (seconds)
        m_alpha = (1 + 10 * diffSec + 0.25 * (diffSec * diffSec));
    }
    m_alpha = 2 * (1 - m_beta) * m_alpha;
    if (m_alpha < 1)
    {
        m_alpha = 1;
    }
    NS_LOG_DEBUG("Updated m_alpha: " << m_alpha);
}

void
TcpHtcpDSS::UpdateBeta()
{
    NS_LOG_FUNCTION(this);

    // Default value for m_beta
    m_beta = m_defaultBackoff;

    if (m_throughput > m_lastThroughput && m_lastThroughput > 0)
    {
        uint32_t diff = m_throughput - m_lastThroughput;
        if (diff / m_lastThroughput <= m_throughputRatio)
        {
            m_beta = m_minRtt.GetDouble() / m_maxRtt.GetDouble();
        }
    }
    NS_LOG_DEBUG("Updated m_beta: " << m_beta);
}

uint32_t
TcpHtcpDSS::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this << tcb << bytesInFlight);

    m_lastCon = Simulator::Now();

    UpdateBeta();
    UpdateAlpha();

    uint32_t segWin = 2 * tcb->m_segmentSize;
    auto bFlight = static_cast<uint32_t>(bytesInFlight * m_beta);
    uint32_t ssThresh = std::max(segWin, bFlight);
    m_minRtt = Time::Max();
    m_maxRtt = Time::Min();
    m_lastThroughput = m_throughput;
    m_throughput = 0;
    m_dataSent = 0;
    NS_LOG_DEBUG(this << " ssThresh: " << ssThresh << " m_beta: " << m_beta);
    return ssThresh;
}

void
TcpHtcpDSS::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked << rtt);
    NS_LOG_DEBUG("TcpSocketState: " << tcb->m_congState);
    if (tcb->m_congState == TcpSocketState::CA_OPEN)
    {
        m_dataSent += segmentsAcked * tcb->m_segmentSize;
    }

    m_throughput = static_cast<uint32_t>(m_dataSent /
                                         (Simulator::Now().GetSeconds() - m_lastCon.GetSeconds()));

    UpdateAlpha();
    if (rtt < m_minRtt)
    {
        m_minRtt = rtt;
        NS_LOG_DEBUG("Updated m_minRtt=" << m_minRtt);
    }
    if (rtt > m_maxRtt)
    {
        m_maxRtt = rtt;
        NS_LOG_DEBUG("Updated m_maxRtt=" << m_maxRtt);
    }

    // New Dynamic Slow Start Adjustments
    if (tcb->m_cWnd < tcb->m_ssThresh)  // Still in slow start phase
    {
        // Estimate bandwidth based on RTT
        double bandwidthEstimate = 1.0 / rtt.GetSeconds();  // Simple bandwidth estimate (inverse of RTT)
        
        if (bandwidthEstimate > m_rttThresholdLow)
        {
            tcb->m_cWnd += tcb->m_segmentSize * 2;  // Grow faster if RTT is low (high bandwidth)
        }
        else if (bandwidthEstimate > m_rttThresholdHigh)
        {
            tcb->m_cWnd += tcb->m_segmentSize * 1.5;  // Moderate growth if RTT is medium
        }
        else
        {
            tcb->m_cWnd += tcb->m_segmentSize;  // Slow growth if RTT is high (low bandwidth)
        }

        NS_LOG_DEBUG("Dynamic Slow Start: Updated cwnd to " << tcb->m_cWnd);
    }
}

} // namespace ns3
