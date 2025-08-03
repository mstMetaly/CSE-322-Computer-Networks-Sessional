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

#ifndef TCP_HTCP_DSS_H
#define TCP_HTCP_DSS_H

#include "tcp-congestion-ops.h"

namespace ns3
{

class TcpSocketState;

/**
 * \ingroup congestionOps
 *
 * \brief An implementation of the H-TCP with Dynamic Slow Start (DSS) variant of TCP.
 *
 * This class contains the updated H-TCP implementation, adding Dynamic Slow Start (DSS) for improved performance
 * in high-speed and long-distance networks. This version adapts the congestion window increase rate based on RTT.
 */
class TcpHtcpDSS : public TcpNewReno
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    
    /**
     * Create an unbound TCP socket.
     */
    TcpHtcpDSS();

    /**
     * \brief Copy constructor
     * \param sock the object to copy
     */
    TcpHtcpDSS(const TcpHtcpDSS& sock);
    
    ~TcpHtcpDSS() override;

    std::string GetName() const override;
    
    Ptr<TcpCongestionOps> Fork() override;

    uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) override;

    void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt) override;

  protected:
    void CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;

  private:
    /**
     * \brief Updates the additive increase parameter for H-TCP with DSS
     */
    void UpdateAlpha();

    /**
     * \brief Updates the multiplicative decrease factor beta for H-TCP with DSS
     */
    void UpdateBeta();

    // H-TCP with DSS variables
    double m_alpha;           //!< AIMD additive increase parameter
    double m_beta;            //!< AIMD multiplicative decrease factor
    double m_defaultBackoff;  //!< Default value when throughput ratio is less than default
    double m_throughputRatio; //!< Ratio of two consecutive throughput
    Time m_delta;             //!< Time in seconds that has elapsed since the last congestion event
    Time m_deltaL;            //!< Threshold for switching between standard and new increase function
    Time m_lastCon;           //!< Time of the last congestion for the flow
    Time m_minRtt;            //!< Minimum RTT in each congestion period
    Time m_maxRtt;            //!< Maximum RTT in each congestion period
    uint32_t m_throughput;    //!< Current throughput since the last congestion
    uint32_t m_lastThroughput; //!< Throughput in the last congestion period
    uint32_t m_dataSent;      //!< Current amount of data sent since the last congestion

    // New parameters for Dynamic Slow Start
    double m_rttThresholdHigh; //!< High RTT threshold (for fast growth)
    double m_rttThresholdLow;  //!< Low RTT threshold (for moderate growth)
};

} // namespace ns3

#endif /* TCP_HTCP_DSS_H */
