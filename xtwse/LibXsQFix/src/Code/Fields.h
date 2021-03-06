/* -*- C++ -*- */
 
/****************************************************************************
** Copyright (c) quickfixengine.org  All rights reserved.
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/
 
#ifndef FIX_FIELDS_H
#define FIX_FIELDS_H

#include "DeprecatedFields.h"
#include "Field.h"

#undef Yield

namespace Kway
{
namespace FIX
{
  
 DEFINE_STRING(Account);
 DEFINE_STRING(AdvId);
 DEFINE_STRING(AdvRefID);
 DEFINE_CHAR(AdvSide);
 DEFINE_STRING(AdvTransType);
 DEFINE_PRICE(AvgPx);
 DEFINE_SEQNUM(BeginSeqNo);
 DEFINE_STRING(BeginString);
 DEFINE_LENGTH(BodyLength);
 DEFINE_CHECKSUM(CheckSum);
 DEFINE_STRING(ClOrdID);
 DEFINE_AMT(Commission);
 DEFINE_CHAR(CommType);
 DEFINE_QTY(CumQty);
 DEFINE_CURRENCY(Currency);
 DEFINE_SEQNUM(EndSeqNo);
 DEFINE_STRING(ExecID);
 DEFINE_MULTIPLEVALUESTRING(ExecInst);
 DEFINE_STRING(ExecRefID);
 DEFINE_CHAR(HandlInst);
 DEFINE_STRING(SecurityIDSource);
 DEFINE_STRING(IOIid);
 DEFINE_CHAR(IOIQltyInd);
 DEFINE_STRING(IOIRefID);
 DEFINE_STRING(IOIQty);
 DEFINE_CHAR(IOITransType);
 DEFINE_CHAR(LastCapacity);
 DEFINE_EXCHANGE(LastMkt);
 DEFINE_PRICE(LastPx);
 DEFINE_QTY(LastQty);
 DEFINE_NUMINGROUP(LinesOfText);
 DEFINE_SEQNUM(MsgSeqNum);
 DEFINE_STRING(MsgType);
 DEFINE_SEQNUM(NewSeqNo);
 DEFINE_STRING(OrderID);
 DEFINE_QTY(OrderQty);
 DEFINE_CHAR(OrdStatus);
 DEFINE_CHAR(OrdType);
 DEFINE_STRING(OrigClOrdID);
 DEFINE_UTCTIMESTAMP(OrigTime);
 DEFINE_BOOLEAN(PossDupFlag);
 DEFINE_PRICE(Price);
 DEFINE_SEQNUM(RefSeqNum);
 DEFINE_STRING(SecurityID);
 DEFINE_STRING(SenderCompID);
 DEFINE_STRING(SenderSubID);
 DEFINE_UTCTIMESTAMP(SendingTime);
 DEFINE_QTY(Quantity);
 DEFINE_CHAR(Side);
 DEFINE_STRING(Symbol);
 DEFINE_STRING(TargetCompID);
 DEFINE_STRING(TargetSubID);
 DEFINE_STRING(Text);
 DEFINE_CHAR(TimeInForce);
 DEFINE_UTCTIMESTAMP(TransactTime);
 DEFINE_CHAR(Urgency);
 DEFINE_UTCTIMESTAMP(ValidUntilTime);
 DEFINE_CHAR(SettlType);
 DEFINE_LOCALMKTDATE(SettlDate);
 DEFINE_STRING(SymbolSfx);
 DEFINE_STRING(ListID);
 DEFINE_INT(ListSeqNo);
 DEFINE_INT(TotNoOrders);
 DEFINE_INT(ListNoOrds);
 DEFINE_STRING(ListExecInst);
 DEFINE_STRING(AllocID);
 DEFINE_CHAR(AllocTransType);
 DEFINE_STRING(RefAllocID);
 DEFINE_NUMINGROUP(NoOrders);
 DEFINE_INT(AvgPxPrecision);
 DEFINE_LOCALMKTDATE(TradeDate);
 DEFINE_CHAR(PositionEffect);
 DEFINE_NUMINGROUP(NoAllocs);
 DEFINE_STRING(AllocAccount);
 DEFINE_QTY(AllocQty);
 DEFINE_CHAR(ProcessCode);
 DEFINE_NUMINGROUP(NoRpts);
 DEFINE_INT(RptSeq);
 DEFINE_QTY(CxlQty);
 DEFINE_NUMINGROUP(NoDlvyInst);
 DEFINE_INT(AllocStatus);
 DEFINE_INT(AllocRejCode);
 DEFINE_DATA(Signature);
 DEFINE_LENGTH(SecureDataLen);
 DEFINE_DATA(SecureData);
 DEFINE_LENGTH(SignatureLength);
 DEFINE_CHAR(EmailType);
 DEFINE_LENGTH(RawDataLength);
 DEFINE_DATA(RawData);
 DEFINE_BOOLEAN(PossResend);
 DEFINE_INT(EncryptMethod);
 DEFINE_PRICE(StopPx);
 DEFINE_EXCHANGE(ExDestination);
 DEFINE_INT(CxlRejReason);
 DEFINE_INT(OrdRejReason);
 DEFINE_CHAR(IOIQualifier);
 DEFINE_STRING(WaveNo);
 DEFINE_STRING(Issuer);
 DEFINE_STRING(SecurityDesc);
 DEFINE_INT(HeartBtInt);
 DEFINE_QTY(MinQty);
 DEFINE_QTY(MaxFloor);
 DEFINE_STRING(TestReqID);
 DEFINE_BOOLEAN(ReportToExch);
 DEFINE_BOOLEAN(LocateReqd);
 DEFINE_STRING(OnBehalfOfCompID);
 DEFINE_STRING(OnBehalfOfSubID);
 DEFINE_STRING(QuoteID);
 DEFINE_AMT(NetMoney);
 DEFINE_AMT(SettlCurrAmt);
 DEFINE_CURRENCY(SettlCurrency);
 DEFINE_BOOLEAN(ForexReq);
 DEFINE_UTCTIMESTAMP(OrigSendingTime);
 DEFINE_BOOLEAN(GapFillFlag);
 DEFINE_NUMINGROUP(NoExecs);
 DEFINE_UTCTIMESTAMP(ExpireTime);
 DEFINE_CHAR(DKReason);
 DEFINE_STRING(DeliverToCompID);
 DEFINE_STRING(DeliverToSubID);
 DEFINE_BOOLEAN(IOINaturalFlag);
 DEFINE_STRING(QuoteReqID);
 DEFINE_PRICE(BidPx);
 DEFINE_PRICE(OfferPx);
 DEFINE_QTY(BidSize);
 DEFINE_QTY(OfferSize);
 DEFINE_NUMINGROUP(NoMiscFees);
 DEFINE_AMT(MiscFeeAmt);
 DEFINE_CURRENCY(MiscFeeCurr);
 DEFINE_CHAR(MiscFeeType);
 DEFINE_PRICE(PrevClosePx);
 DEFINE_BOOLEAN(ResetSeqNumFlag);
 DEFINE_STRING(SenderLocationID);
 DEFINE_STRING(TargetLocationID);
 DEFINE_STRING(OnBehalfOfLocationID);
 DEFINE_STRING(DeliverToLocationID);
 DEFINE_NUMINGROUP(NoRelatedSym);
 DEFINE_STRING(Subject);
 DEFINE_STRING(Headline);
 DEFINE_STRING(URLLink);
 DEFINE_CHAR(ExecType);
 DEFINE_QTY(LeavesQty);
 DEFINE_QTY(CashOrderQty);
 DEFINE_PRICE(AllocAvgPx);
 DEFINE_AMT(AllocNetMoney);
 DEFINE_FLOAT(SettlCurrFxRate);
 DEFINE_CHAR(SettlCurrFxRateCalc);
 DEFINE_INT(NumDaysInterest);
 DEFINE_PERCENTAGE(AccruedInterestRate);
 DEFINE_AMT(AccruedInterestAmt);
 DEFINE_CHAR(SettlInstMode);
 DEFINE_STRING(AllocText);
 DEFINE_STRING(SettlInstID);
 DEFINE_CHAR(SettlInstTransType);
 DEFINE_STRING(EmailThreadID);
 DEFINE_CHAR(SettlInstSource);
 DEFINE_STRING(SecurityType);
 DEFINE_UTCTIMESTAMP(EffectiveTime);
 DEFINE_INT(StandInstDbType);
 DEFINE_STRING(StandInstDbName);
 DEFINE_STRING(StandInstDbID);
 DEFINE_INT(SettlDeliveryType);
 DEFINE_PRICE(BidSpotRate);
 DEFINE_PRICEOFFSET(BidForwardPoints);
 DEFINE_PRICE(OfferSpotRate);
 DEFINE_PRICEOFFSET(OfferForwardPoints);
 DEFINE_QTY(OrderQty2);
 DEFINE_LOCALMKTDATE(SettlDate2);
 DEFINE_PRICE(LastSpotRate);
 DEFINE_PRICEOFFSET(LastForwardPoints);
 DEFINE_STRING(AllocLinkID);
 DEFINE_INT(AllocLinkType);
 DEFINE_STRING(SecondaryOrderID);
 DEFINE_NUMINGROUP(NoIOIQualifiers);
 DEFINE_MONTHYEAR(MaturityMonthYear);
 DEFINE_PRICE(StrikePrice);
 DEFINE_INT(CoveredOrUncovered);
 DEFINE_CHAR(OptAttribute);
 DEFINE_EXCHANGE(SecurityExchange);
 DEFINE_BOOLEAN(NotifyBrokerOfCredit);
 DEFINE_INT(AllocHandlInst);
 DEFINE_QTY(MaxShow);
 DEFINE_FLOAT(PegOffsetValue);
 DEFINE_LENGTH(XmlDataLen);
 DEFINE_DATA(XmlData);
 DEFINE_STRING(SettlInstRefID);
 DEFINE_NUMINGROUP(NoRoutingIDs);
 DEFINE_INT(RoutingType);
 DEFINE_STRING(RoutingID);
 DEFINE_PRICEOFFSET(Spread);
 DEFINE_CURRENCY(BenchmarkCurveCurrency);
 DEFINE_STRING(BenchmarkCurveName);
 DEFINE_STRING(BenchmarkCurvePoint);
 DEFINE_PERCENTAGE(CouponRate);
 DEFINE_LOCALMKTDATE(CouponPaymentDate);
 DEFINE_LOCALMKTDATE(IssueDate);
 DEFINE_INT(RepurchaseTerm);
 DEFINE_PERCENTAGE(RepurchaseRate);
 DEFINE_FLOAT(Factor);
 DEFINE_LOCALMKTDATE(TradeOriginationDate);
 DEFINE_LOCALMKTDATE(ExDate);
 DEFINE_FLOAT(ContractMultiplier);
 DEFINE_NUMINGROUP(NoStipulations);
 DEFINE_STRING(StipulationType);
 DEFINE_STRING(StipulationValue);
 DEFINE_STRING(YieldType);
 DEFINE_PERCENTAGE(Yield);
 DEFINE_AMT(TotalTakedown);
 DEFINE_AMT(Concession);
 DEFINE_INT(RepoCollateralSecurityType);
 DEFINE_LOCALMKTDATE(RedemptionDate);
 DEFINE_LOCALMKTDATE(UnderlyingCouponPaymentDate);
 DEFINE_LOCALMKTDATE(UnderlyingIssueDate);
 DEFINE_INT(UnderlyingRepoCollateralSecurityType);
 DEFINE_INT(UnderlyingRepurchaseTerm);
 DEFINE_PERCENTAGE(UnderlyingRepurchaseRate);
 DEFINE_FLOAT(UnderlyingFactor);
 DEFINE_LOCALMKTDATE(UnderlyingRedemptionDate);
 DEFINE_LOCALMKTDATE(LegCouponPaymentDate);
 DEFINE_LOCALMKTDATE(LegIssueDate);
 DEFINE_INT(LegRepoCollateralSecurityType);
 DEFINE_INT(LegRepurchaseTerm);
 DEFINE_PERCENTAGE(LegRepurchaseRate);
 DEFINE_FLOAT(LegFactor);
 DEFINE_LOCALMKTDATE(LegRedemptionDate);
 DEFINE_STRING(CreditRating);
 DEFINE_STRING(UnderlyingCreditRating);
 DEFINE_STRING(LegCreditRating);
 DEFINE_BOOLEAN(TradedFlatSwitch);
 DEFINE_LOCALMKTDATE(BasisFeatureDate);
 DEFINE_PRICE(BasisFeaturePrice);
 DEFINE_STRING(MDReqID);
 DEFINE_CHAR(SubscriptionRequestType);
 DEFINE_INT(MarketDepth);
 DEFINE_INT(MDUpdateType);
 DEFINE_BOOLEAN(AggregatedBook);
 DEFINE_NUMINGROUP(NoMDEntryTypes);
 DEFINE_NUMINGROUP(NoMDEntries);
 DEFINE_CHAR(MDEntryType);
 DEFINE_PRICE(MDEntryPx);
 DEFINE_QTY(MDEntrySize);
 DEFINE_UTCDATEONLY(MDEntryDate);
 DEFINE_UTCTIMEONLY(MDEntryTime);
 DEFINE_CHAR(TickDirection);
 DEFINE_EXCHANGE(MDMkt);
 DEFINE_MULTIPLEVALUESTRING(QuoteCondition);
 DEFINE_MULTIPLEVALUESTRING(TradeCondition);
 DEFINE_STRING(MDEntryID);
 DEFINE_CHAR(MDUpdateAction);
 DEFINE_STRING(MDEntryRefID);
 DEFINE_CHAR(MDReqRejReason);
 DEFINE_STRING(MDEntryOriginator);
 DEFINE_STRING(LocationID);
 DEFINE_STRING(DeskID);
 DEFINE_CHAR(DeleteReason);
 DEFINE_MULTIPLEVALUESTRING(OpenCloseSettlFlag);
 DEFINE_INT(SellerDays);
 DEFINE_STRING(MDEntryBuyer);
 DEFINE_STRING(MDEntrySeller);
 DEFINE_INT(MDEntryPositionNo);
 DEFINE_MULTIPLEVALUESTRING(FinancialStatus);
 DEFINE_MULTIPLEVALUESTRING(CorporateAction);
 DEFINE_QTY(DefBidSize);
 DEFINE_QTY(DefOfferSize);
 DEFINE_NUMINGROUP(NoQuoteEntries);
 DEFINE_NUMINGROUP(NoQuoteSets);
 DEFINE_INT(QuoteStatus);
 DEFINE_INT(QuoteCancelType);
 DEFINE_STRING(QuoteEntryID);
 DEFINE_INT(QuoteRejectReason);
 DEFINE_INT(QuoteResponseLevel);
 DEFINE_STRING(QuoteSetID);
 DEFINE_INT(QuoteRequestType);
 DEFINE_INT(TotNoQuoteEntries);
 DEFINE_STRING(UnderlyingSecurityIDSource);
 DEFINE_STRING(UnderlyingIssuer);
 DEFINE_STRING(UnderlyingSecurityDesc);
 DEFINE_EXCHANGE(UnderlyingSecurityExchange);
 DEFINE_STRING(UnderlyingSecurityID);
 DEFINE_STRING(UnderlyingSecurityType);
 DEFINE_STRING(UnderlyingSymbol);
 DEFINE_STRING(UnderlyingSymbolSfx);
 DEFINE_MONTHYEAR(UnderlyingMaturityMonthYear);
 DEFINE_PRICE(UnderlyingStrikePrice);
 DEFINE_CHAR(UnderlyingOptAttribute);
 DEFINE_CURRENCY(UnderlyingCurrency);
 DEFINE_STRING(SecurityReqID);
 DEFINE_INT(SecurityRequestType);
 DEFINE_STRING(SecurityResponseID);
 DEFINE_INT(SecurityResponseType);
 DEFINE_STRING(SecurityStatusReqID);
 DEFINE_BOOLEAN(UnsolicitedIndicator);
 DEFINE_INT(SecurityTradingStatus);
 DEFINE_CHAR(HaltReason);
 DEFINE_BOOLEAN(InViewOfCommon);
 DEFINE_BOOLEAN(DueToRelated);
 DEFINE_QTY(BuyVolume);
 DEFINE_QTY(SellVolume);
 DEFINE_PRICE(HighPx);
 DEFINE_PRICE(LowPx);
 DEFINE_INT(Adjustment);
 DEFINE_STRING(TradSesReqID);
 DEFINE_STRING(TradingSessionID);
 DEFINE_STRING(ContraTrader);
 DEFINE_INT(TradSesMethod);
 DEFINE_INT(TradSesMode);
 DEFINE_INT(TradSesStatus);
 DEFINE_UTCTIMESTAMP(TradSesStartTime);
 DEFINE_UTCTIMESTAMP(TradSesOpenTime);
 DEFINE_UTCTIMESTAMP(TradSesPreCloseTime);
 DEFINE_UTCTIMESTAMP(TradSesCloseTime);
 DEFINE_UTCTIMESTAMP(TradSesEndTime);
 DEFINE_INT(NumberOfOrders);
 DEFINE_STRING(MessageEncoding);
 DEFINE_LENGTH(EncodedIssuerLen);
 DEFINE_DATA(EncodedIssuer);
 DEFINE_LENGTH(EncodedSecurityDescLen);
 DEFINE_DATA(EncodedSecurityDesc);
 DEFINE_LENGTH(EncodedListExecInstLen);
 DEFINE_DATA(EncodedListExecInst);
 DEFINE_LENGTH(EncodedTextLen);
 DEFINE_DATA(EncodedText);
 DEFINE_LENGTH(EncodedSubjectLen);
 DEFINE_DATA(EncodedSubject);
 DEFINE_LENGTH(EncodedHeadlineLen);
 DEFINE_DATA(EncodedHeadline);
 DEFINE_LENGTH(EncodedAllocTextLen);
 DEFINE_DATA(EncodedAllocText);
 DEFINE_LENGTH(EncodedUnderlyingIssuerLen);
 DEFINE_DATA(EncodedUnderlyingIssuer);
 DEFINE_LENGTH(EncodedUnderlyingSecurityDescLen);
 DEFINE_DATA(EncodedUnderlyingSecurityDesc);
 DEFINE_PRICE(AllocPrice);
 DEFINE_UTCTIMESTAMP(QuoteSetValidUntilTime);
 DEFINE_INT(QuoteEntryRejectReason);
 DEFINE_SEQNUM(LastMsgSeqNumProcessed);
 DEFINE_INT(RefTagID);
 DEFINE_STRING(RefMsgType);
 DEFINE_INT(SessionRejectReason);
 DEFINE_CHAR(BidRequestTransType);
 DEFINE_STRING(ContraBroker);
 DEFINE_STRING(ComplianceID);
 DEFINE_BOOLEAN(SolicitedFlag);
 DEFINE_INT(ExecRestatementReason);
 DEFINE_STRING(BusinessRejectRefID);
 DEFINE_INT(BusinessRejectReason);
 DEFINE_AMT(GrossTradeAmt);
 DEFINE_NUMINGROUP(NoContraBrokers);
 DEFINE_LENGTH(MaxMessageSize);
 DEFINE_NUMINGROUP(NoMsgTypes);
 DEFINE_CHAR(MsgDirection);
 DEFINE_NUMINGROUP(NoTradingSessions);
 DEFINE_QTY(TotalVolumeTraded);
 DEFINE_CHAR(DiscretionInst);
 DEFINE_FLOAT(DiscretionOffsetValue);
 DEFINE_STRING(BidID);
 DEFINE_STRING(ClientBidID);
 DEFINE_STRING(ListName);
 DEFINE_INT(TotNoRelatedSym);
 DEFINE_INT(BidType);
 DEFINE_INT(NumTickets);
 DEFINE_AMT(SideValue1);
 DEFINE_AMT(SideValue2);
 DEFINE_NUMINGROUP(NoBidDescriptors);
 DEFINE_INT(BidDescriptorType);
 DEFINE_STRING(BidDescriptor);
 DEFINE_INT(SideValueInd);
 DEFINE_PERCENTAGE(LiquidityPctLow);
 DEFINE_PERCENTAGE(LiquidityPctHigh);
 DEFINE_AMT(LiquidityValue);
 DEFINE_PERCENTAGE(EFPTrackingError);
 DEFINE_AMT(FairValue);
 DEFINE_PERCENTAGE(OutsideIndexPct);
 DEFINE_AMT(ValueOfFutures);
 DEFINE_INT(LiquidityIndType);
 DEFINE_PERCENTAGE(WtAverageLiquidity);
 DEFINE_BOOLEAN(ExchangeForPhysical);
 DEFINE_AMT(OutMainCntryUIndex);
 DEFINE_PERCENTAGE(CrossPercent);
 DEFINE_INT(ProgRptReqs);
 DEFINE_INT(ProgPeriodInterval);
 DEFINE_INT(IncTaxInd);
 DEFINE_INT(NumBidders);
 DEFINE_CHAR(BidTradeType);
 DEFINE_CHAR(BasisPxType);
 DEFINE_NUMINGROUP(NoBidComponents);
 DEFINE_COUNTRY(Country);
 DEFINE_INT(TotNoStrikes);
 DEFINE_INT(PriceType);
 DEFINE_QTY(DayOrderQty);
 DEFINE_QTY(DayCumQty);
 DEFINE_PRICE(DayAvgPx);
 DEFINE_INT(GTBookingInst);
 DEFINE_NUMINGROUP(NoStrikes);
 DEFINE_INT(ListStatusType);
 DEFINE_INT(NetGrossInd);
 DEFINE_INT(ListOrderStatus);
 DEFINE_LOCALMKTDATE(ExpireDate);
 DEFINE_CHAR(ListExecInstType);
 DEFINE_CHAR(CxlRejResponseTo);
 DEFINE_PERCENTAGE(UnderlyingCouponRate);
 DEFINE_FLOAT(UnderlyingContractMultiplier);
 DEFINE_QTY(ContraTradeQty);
 DEFINE_UTCTIMESTAMP(ContraTradeTime);
 DEFINE_INT(LiquidityNumSecurities);
 DEFINE_CHAR(MultiLegReportingType);
 DEFINE_UTCTIMESTAMP(StrikeTime);
 DEFINE_STRING(ListStatusText);
 DEFINE_LENGTH(EncodedListStatusTextLen);
 DEFINE_DATA(EncodedListStatusText);
 DEFINE_CHAR(PartyIDSource);
 DEFINE_STRING(PartyID);
 DEFINE_PRICEOFFSET(NetChgPrevDay);
 DEFINE_INT(PartyRole);
 DEFINE_NUMINGROUP(NoPartyIDs);
 DEFINE_NUMINGROUP(NoSecurityAltID);
 DEFINE_STRING(SecurityAltID);
 DEFINE_STRING(SecurityAltIDSource);
 DEFINE_NUMINGROUP(NoUnderlyingSecurityAltID);
 DEFINE_STRING(UnderlyingSecurityAltID);
 DEFINE_STRING(UnderlyingSecurityAltIDSource);
 DEFINE_INT(Product);
 DEFINE_STRING(CFICode);
 DEFINE_INT(UnderlyingProduct);
 DEFINE_STRING(UnderlyingCFICode);
 DEFINE_BOOLEAN(TestMessageIndicator);
 DEFINE_INT(QuantityType);
 DEFINE_STRING(BookingRefID);
 DEFINE_STRING(IndividualAllocID);
 DEFINE_CHAR(RoundingDirection);
 DEFINE_FLOAT(RoundingModulus);
 DEFINE_COUNTRY(CountryOfIssue);
 DEFINE_STRING(StateOrProvinceOfIssue);
 DEFINE_STRING(LocaleOfIssue);
 DEFINE_NUMINGROUP(NoRegistDtls);
 DEFINE_STRING(MailingDtls);
 DEFINE_COUNTRY(InvestorCountryOfResidence);
 DEFINE_STRING(PaymentRef);
 DEFINE_INT(DistribPaymentMethod);
 DEFINE_CURRENCY(CashDistribCurr);
 DEFINE_CURRENCY(CommCurrency);
 DEFINE_CHAR(CancellationRights);
 DEFINE_CHAR(MoneyLaunderingStatus);
 DEFINE_STRING(MailingInst);
 DEFINE_UTCTIMESTAMP(TransBkdTime);
 DEFINE_CHAR(ExecPriceType);
 DEFINE_FLOAT(ExecPriceAdjustment);
 DEFINE_LOCALMKTDATE(DateOfBirth);
 DEFINE_INT(TradeReportTransType);
 DEFINE_STRING(CardHolderName);
 DEFINE_STRING(CardNumber);
 DEFINE_LOCALMKTDATE(CardExpDate);
 DEFINE_STRING(CardIssNum);
 DEFINE_INT(PaymentMethod);
 DEFINE_STRING(RegistAcctType);
 DEFINE_STRING(Designation);
 DEFINE_INT(TaxAdvantageType);
 DEFINE_STRING(RegistRejReasonText);
 DEFINE_CHAR(FundRenewWaiv);
 DEFINE_STRING(CashDistribAgentName);
 DEFINE_STRING(CashDistribAgentCode);
 DEFINE_STRING(CashDistribAgentAcctNumber);
 DEFINE_STRING(CashDistribPayRef);
 DEFINE_STRING(CashDistribAgentAcctName);
 DEFINE_LOCALMKTDATE(CardStartDate);
 DEFINE_LOCALMKTDATE(PaymentDate);
 DEFINE_STRING(PaymentRemitterID);
 DEFINE_CHAR(RegistStatus);
 DEFINE_INT(RegistRejReasonCode);
 DEFINE_STRING(RegistRefID);
 DEFINE_STRING(RegistDtls);
 DEFINE_NUMINGROUP(NoDistribInsts);
 DEFINE_STRING(RegistEmail);
 DEFINE_PERCENTAGE(DistribPercentage);
 DEFINE_STRING(RegistID);
 DEFINE_CHAR(RegistTransType);
 DEFINE_UTCTIMESTAMP(ExecValuationPoint);
 DEFINE_PERCENTAGE(OrderPercent);
 DEFINE_CHAR(OwnershipType);
 DEFINE_NUMINGROUP(NoContAmts);
 DEFINE_INT(ContAmtType);
 DEFINE_FLOAT(ContAmtValue);
 DEFINE_CURRENCY(ContAmtCurr);
 DEFINE_INT(OwnerType);
 DEFINE_STRING(PartySubID);
 DEFINE_STRING(NestedPartyID);
 DEFINE_CHAR(NestedPartyIDSource);
 DEFINE_STRING(SecondaryClOrdID);
 DEFINE_STRING(SecondaryExecID);
 DEFINE_CHAR(OrderCapacity);
 DEFINE_MULTIPLEVALUESTRING(OrderRestrictions);
 DEFINE_CHAR(MassCancelRequestType);
 DEFINE_CHAR(MassCancelResponse);
 DEFINE_CHAR(MassCancelRejectReason);
 DEFINE_INT(TotalAffectedOrders);
 DEFINE_INT(NoAffectedOrders);
 DEFINE_STRING(AffectedOrderID);
 DEFINE_STRING(AffectedSecondaryOrderID);
 DEFINE_INT(QuoteType);
 DEFINE_INT(NestedPartyRole);
 DEFINE_NUMINGROUP(NoNestedPartyIDs);
 DEFINE_AMT(TotalAccruedInterestAmt);
 DEFINE_LOCALMKTDATE(MaturityDate);
 DEFINE_LOCALMKTDATE(UnderlyingMaturityDate);
 DEFINE_STRING(InstrRegistry);
 DEFINE_CHAR(CashMargin);
 DEFINE_STRING(NestedPartySubID);
 DEFINE_MULTIPLEVALUESTRING(Scope);
 DEFINE_BOOLEAN(MDImplicitDelete);
 DEFINE_STRING(CrossID);
 DEFINE_INT(CrossType);
 DEFINE_INT(CrossPrioritization);
 DEFINE_STRING(OrigCrossID);
 DEFINE_NUMINGROUP(NoSides);
 DEFINE_STRING(Username);
 DEFINE_STRING(Password);
 DEFINE_NUMINGROUP(NoLegs);
 DEFINE_CURRENCY(LegCurrency);
 DEFINE_INT(TotNoSecurityTypes);
 DEFINE_NUMINGROUP(NoSecurityTypes);
 DEFINE_INT(SecurityListRequestType);
 DEFINE_INT(SecurityRequestResult);
 DEFINE_QTY(RoundLot);
 DEFINE_QTY(MinTradeVol);
 DEFINE_INT(MultiLegRptTypeReq);
 DEFINE_CHAR(LegPositionEffect);
 DEFINE_INT(LegCoveredOrUncovered);
 DEFINE_PRICE(LegPrice);
 DEFINE_INT(TradSesStatusRejReason);
 DEFINE_STRING(TradeRequestID);
 DEFINE_INT(TradeRequestType);
 DEFINE_BOOLEAN(PreviouslyReported);
 DEFINE_STRING(TradeReportID);
 DEFINE_STRING(TradeReportRefID);
 DEFINE_CHAR(MatchStatus);
 DEFINE_STRING(MatchType);
 DEFINE_BOOLEAN(OddLot);
 DEFINE_INT(NoClearingInstructions);
 DEFINE_INT(ClearingInstruction);
 DEFINE_STRING(TradeInputSource);
 DEFINE_STRING(TradeInputDevice);
 DEFINE_INT(NoDates);
 DEFINE_INT(AccountType);
 DEFINE_INT(CustOrderCapacity);
 DEFINE_STRING(ClOrdLinkID);
 DEFINE_STRING(MassStatusReqID);
 DEFINE_INT(MassStatusReqType);
 DEFINE_UTCTIMESTAMP(OrigOrdModTime);
 DEFINE_CHAR(LegSettlType);
 DEFINE_LOCALMKTDATE(LegSettlDate);
 DEFINE_CHAR(DayBookingInst);
 DEFINE_CHAR(BookingUnit);
 DEFINE_CHAR(PreallocMethod);
 DEFINE_COUNTRY(UnderlyingCountryOfIssue);
 DEFINE_STRING(UnderlyingStateOrProvinceOfIssue);
 DEFINE_STRING(UnderlyingLocaleOfIssue);
 DEFINE_STRING(UnderlyingInstrRegistry);
 DEFINE_COUNTRY(LegCountryOfIssue);
 DEFINE_STRING(LegStateOrProvinceOfIssue);
 DEFINE_STRING(LegLocaleOfIssue);
 DEFINE_STRING(LegInstrRegistry);
 DEFINE_STRING(LegSymbol);
 DEFINE_STRING(LegSymbolSfx);
 DEFINE_STRING(LegSecurityID);
 DEFINE_STRING(LegSecurityIDSource);
 DEFINE_STRING(NoLegSecurityAltID);
 DEFINE_STRING(LegSecurityAltID);
 DEFINE_STRING(LegSecurityAltIDSource);
 DEFINE_INT(LegProduct);
 DEFINE_STRING(LegCFICode);
 DEFINE_STRING(LegSecurityType);
 DEFINE_MONTHYEAR(LegMaturityMonthYear);
 DEFINE_LOCALMKTDATE(LegMaturityDate);
 DEFINE_PRICE(LegStrikePrice);
 DEFINE_CHAR(LegOptAttribute);
 DEFINE_FLOAT(LegContractMultiplier);
 DEFINE_PERCENTAGE(LegCouponRate);
 DEFINE_EXCHANGE(LegSecurityExchange);
 DEFINE_STRING(LegIssuer);
 DEFINE_LENGTH(EncodedLegIssuerLen);
 DEFINE_DATA(EncodedLegIssuer);
 DEFINE_STRING(LegSecurityDesc);
 DEFINE_LENGTH(EncodedLegSecurityDescLen);
 DEFINE_DATA(EncodedLegSecurityDesc);
 DEFINE_FLOAT(LegRatioQty);
 DEFINE_CHAR(LegSide);
 DEFINE_STRING(TradingSessionSubID);
 DEFINE_INT(AllocType);
 DEFINE_NUMINGROUP(NoHops);
 DEFINE_STRING(HopCompID);
 DEFINE_UTCTIMESTAMP(HopSendingTime);
 DEFINE_SEQNUM(HopRefID);
 DEFINE_PRICE(MidPx);
 DEFINE_PERCENTAGE(BidYield);
 DEFINE_PERCENTAGE(MidYield);
 DEFINE_PERCENTAGE(OfferYield);
 DEFINE_STRING(ClearingFeeIndicator);
 DEFINE_BOOLEAN(WorkingIndicator);
 DEFINE_PRICE(LegLastPx);
 DEFINE_INT(PriorityIndicator);
 DEFINE_PRICEOFFSET(PriceImprovement);
 DEFINE_PRICE(Price2);
 DEFINE_PRICEOFFSET(LastForwardPoints2);
 DEFINE_PRICEOFFSET(BidForwardPoints2);
 DEFINE_PRICEOFFSET(OfferForwardPoints2);
 DEFINE_STRING(RFQReqID);
 DEFINE_PRICE(MktBidPx);
 DEFINE_PRICE(MktOfferPx);
 DEFINE_QTY(MinBidSize);
 DEFINE_QTY(MinOfferSize);
 DEFINE_STRING(QuoteStatusReqID);
 DEFINE_BOOLEAN(LegalConfirm);
 DEFINE_PRICE(UnderlyingLastPx);
 DEFINE_QTY(UnderlyingLastQty);
 DEFINE_STRING(LegRefID);
 DEFINE_STRING(ContraLegRefID);
 DEFINE_FLOAT(SettlCurrBidFxRate);
 DEFINE_FLOAT(SettlCurrOfferFxRate);
 DEFINE_INT(QuoteRequestRejectReason);
 DEFINE_STRING(SideComplianceID);
 DEFINE_INT(AcctIDSource);
 DEFINE_INT(AllocAcctIDSource);
 DEFINE_PRICE(BenchmarkPrice);
 DEFINE_INT(BenchmarkPriceType);
 DEFINE_STRING(ConfirmID);
 DEFINE_INT(ConfirmStatus);
 DEFINE_INT(ConfirmTransType);
 DEFINE_MONTHYEAR(ContractSettlMonth);
 DEFINE_INT(DeliveryForm);
 DEFINE_PRICE(LastParPx);
 DEFINE_NUMINGROUP(NoLegAllocs);
 DEFINE_STRING(LegAllocAccount);
 DEFINE_STRING(LegIndividualAllocID);
 DEFINE_QTY(LegAllocQty);
 DEFINE_STRING(LegAllocAcctIDSource);
 DEFINE_CURRENCY(LegSettlCurrency);
 DEFINE_CURRENCY(LegBenchmarkCurveCurrency);
 DEFINE_STRING(LegBenchmarkCurveName);
 DEFINE_STRING(LegBenchmarkCurvePoint);
 DEFINE_PRICE(LegBenchmarkPrice);
 DEFINE_INT(LegBenchmarkPriceType);
 DEFINE_PRICE(LegBidPx);
 DEFINE_STRING(LegIOIQty);
 DEFINE_NUMINGROUP(NoLegStipulations);
 DEFINE_PRICE(LegOfferPx);
 DEFINE_QTY(LegOrderQty);
 DEFINE_INT(LegPriceType);
 DEFINE_QTY(LegQty);
 DEFINE_STRING(LegStipulationType);
 DEFINE_STRING(LegStipulationValue);
 DEFINE_INT(LegSwapType);
 DEFINE_STRING(Pool);
 DEFINE_INT(QuotePriceType);
 DEFINE_STRING(QuoteRespID);
 DEFINE_INT(QuoteRespType);
 DEFINE_CHAR(QuoteQualifier);
 DEFINE_LOCALMKTDATE(YieldRedemptionDate);
 DEFINE_PRICE(YieldRedemptionPrice);
 DEFINE_INT(YieldRedemptionPriceType);
 DEFINE_STRING(BenchmarkSecurityID);
 DEFINE_BOOLEAN(ReversalIndicator);
 DEFINE_LOCALMKTDATE(YieldCalcDate);
 DEFINE_NUMINGROUP(NoPositions);
 DEFINE_STRING(PosType);
 DEFINE_QTY(LongQty);
 DEFINE_QTY(ShortQty);
 DEFINE_INT(PosQtyStatus);
 DEFINE_STRING(PosAmtType);
 DEFINE_AMT(PosAmt);
 DEFINE_INT(PosTransType);
 DEFINE_STRING(PosReqID);
 DEFINE_NUMINGROUP(NoUnderlyings);
 DEFINE_INT(PosMaintAction);
 DEFINE_STRING(OrigPosReqRefID);
 DEFINE_STRING(PosMaintRptRefID);
 DEFINE_LOCALMKTDATE(ClearingBusinessDate);
 DEFINE_STRING(SettlSessID);
 DEFINE_STRING(SettlSessSubID);
 DEFINE_INT(AdjustmentType);
 DEFINE_BOOLEAN(ContraryInstructionIndicator);
 DEFINE_BOOLEAN(PriorSpreadIndicator);
 DEFINE_STRING(PosMaintRptID);
 DEFINE_INT(PosMaintStatus);
 DEFINE_INT(PosMaintResult);
 DEFINE_INT(PosReqType);
 DEFINE_INT(ResponseTransportType);
 DEFINE_STRING(ResponseDestination);
 DEFINE_INT(TotalNumPosReports);
 DEFINE_INT(PosReqResult);
 DEFINE_INT(PosReqStatus);
 DEFINE_PRICE(SettlPrice);
 DEFINE_INT(SettlPriceType);
 DEFINE_PRICE(UnderlyingSettlPrice);
 DEFINE_INT(UnderlyingSettlPriceType);
 DEFINE_PRICE(PriorSettlPrice);
 DEFINE_NUMINGROUP(NoQuoteQualifiers);
 DEFINE_CURRENCY(AllocSettlCurrency);
 DEFINE_AMT(AllocSettlCurrAmt);
 DEFINE_AMT(InterestAtMaturity);
 DEFINE_LOCALMKTDATE(LegDatedDate);
 DEFINE_STRING(LegPool);
 DEFINE_AMT(AllocInterestAtMaturity);
 DEFINE_AMT(AllocAccruedInterestAmt);
 DEFINE_LOCALMKTDATE(DeliveryDate);
 DEFINE_CHAR(AssignmentMethod);
 DEFINE_QTY(AssignmentUnit);
 DEFINE_AMT(OpenInterest);
 DEFINE_CHAR(ExerciseMethod);
 DEFINE_INT(TotNumTradeReports);
 DEFINE_INT(TradeRequestResult);
 DEFINE_INT(TradeRequestStatus);
 DEFINE_INT(TradeReportRejectReason);
 DEFINE_INT(SideMultiLegReportingType);
 DEFINE_NUMINGROUP(NoPosAmt);
 DEFINE_BOOLEAN(AutoAcceptIndicator);
 DEFINE_STRING(AllocReportID);
 DEFINE_NUMINGROUP(NoNested2PartyIDs);
 DEFINE_STRING(Nested2PartyID);
 DEFINE_CHAR(Nested2PartyIDSource);
 DEFINE_INT(Nested2PartyRole);
 DEFINE_STRING(Nested2PartySubID);
 DEFINE_STRING(BenchmarkSecurityIDSource);
 DEFINE_STRING(SecuritySubType);
 DEFINE_STRING(UnderlyingSecuritySubType);
 DEFINE_STRING(LegSecuritySubType);
 DEFINE_PERCENTAGE(AllowableOneSidednessPct);
 DEFINE_AMT(AllowableOneSidednessValue);
 DEFINE_CURRENCY(AllowableOneSidednessCurr);
 DEFINE_NUMINGROUP(NoTrdRegTimestamps);
 DEFINE_UTCTIMESTAMP(TrdRegTimestamp);
 DEFINE_INT(TrdRegTimestampType);
 DEFINE_STRING(TrdRegTimestampOrigin);
 DEFINE_STRING(ConfirmRefID);
 DEFINE_INT(ConfirmType);
 DEFINE_INT(ConfirmRejReason);
 DEFINE_INT(BookingType);
 DEFINE_INT(IndividualAllocRejCode);
 DEFINE_STRING(SettlInstMsgID);
 DEFINE_NUMINGROUP(NoSettlInst);
 DEFINE_UTCTIMESTAMP(LastUpdateTime);
 DEFINE_INT(AllocSettlInstType);
 DEFINE_NUMINGROUP(NoSettlPartyIDs);
 DEFINE_STRING(SettlPartyID);
 DEFINE_CHAR(SettlPartyIDSource);
 DEFINE_INT(SettlPartyRole);
 DEFINE_STRING(SettlPartySubID);
 DEFINE_INT(SettlPartySubIDType);
 DEFINE_CHAR(DlvyInstType);
 DEFINE_INT(TerminationType);
 DEFINE_SEQNUM(NextExpectedMsgSeqNum);
 DEFINE_STRING(OrdStatusReqID);
 DEFINE_STRING(SettlInstReqID);
 DEFINE_INT(SettlInstReqRejCode);
 DEFINE_STRING(SecondaryAllocID);
 DEFINE_INT(AllocReportType);
 DEFINE_STRING(AllocReportRefID);
 DEFINE_INT(AllocCancReplaceReason);
 DEFINE_BOOLEAN(CopyMsgIndicator);
 DEFINE_INT(AllocAccountType);
 DEFINE_PRICE(OrderAvgPx);
 DEFINE_QTY(OrderBookingQty);
 DEFINE_NUMINGROUP(NoSettlPartySubIDs);
 DEFINE_NUMINGROUP(NoPartySubIDs);
 DEFINE_INT(PartySubIDType);
 DEFINE_NUMINGROUP(NoNestedPartySubIDs);
 DEFINE_INT(NestedPartySubIDType);
 DEFINE_NUMINGROUP(NoNested2PartySubIDs);
 DEFINE_INT(Nested2PartySubIDType);
 DEFINE_INT(AllocIntermedReqType);
 DEFINE_PRICE(UnderlyingPx);
 DEFINE_FLOAT(PriceDelta);
 DEFINE_INT(ApplQueueMax);
 DEFINE_INT(ApplQueueDepth);
 DEFINE_INT(ApplQueueResolution);
 DEFINE_INT(ApplQueueAction);
 DEFINE_NUMINGROUP(NoAltMDSource);
 DEFINE_STRING(AltMDSourceID);
 DEFINE_STRING(SecondaryTradeReportID);
 DEFINE_INT(AvgPxIndicator);
 DEFINE_STRING(TradeLinkID);
 DEFINE_STRING(OrderInputDevice);
 DEFINE_STRING(UnderlyingTradingSessionID);
 DEFINE_STRING(UnderlyingTradingSessionSubID);
 DEFINE_STRING(TradeLegRefID);
 DEFINE_STRING(ExchangeRule);
 DEFINE_INT(TradeAllocIndicator);
 DEFINE_INT(ExpirationCycle);
 DEFINE_INT(TrdType);
 DEFINE_INT(TrdSubType);
 DEFINE_STRING(TransferReason);
 DEFINE_STRING(AsgnReqID);
 DEFINE_INT(TotNumAssignmentReports);
 DEFINE_STRING(AsgnRptID);
 DEFINE_PRICEOFFSET(ThresholdAmount);
 DEFINE_INT(PegMoveType);
 DEFINE_INT(PegOffsetType);
 DEFINE_INT(PegLimitType);
 DEFINE_INT(PegRoundDirection);
 DEFINE_PRICE(PeggedPrice);
 DEFINE_INT(PegScope);
 DEFINE_INT(DiscretionMoveType);
 DEFINE_INT(DiscretionOffsetType);
 DEFINE_INT(DiscretionLimitType);
 DEFINE_INT(DiscretionRoundDirection);
 DEFINE_PRICE(DiscretionPrice);
 DEFINE_INT(DiscretionScope);
 DEFINE_INT(TargetStrategy);
 DEFINE_STRING(TargetStrategyParameters);
 DEFINE_PERCENTAGE(ParticipationRate);
 DEFINE_FLOAT(TargetStrategyPerformance);
 DEFINE_INT(LastLiquidityInd);
 DEFINE_BOOLEAN(PublishTrdIndicator);
 DEFINE_INT(ShortSaleReason);
 DEFINE_INT(QtyType);
 DEFINE_INT(SecondaryTrdType);
 DEFINE_INT(TradeReportType);
 DEFINE_INT(AllocNoOrdersType);
 DEFINE_AMT(SharedCommission);
 DEFINE_STRING(ConfirmReqID);
 DEFINE_PRICE(AvgParPx);
 DEFINE_PRICE(ReportedPx);
 DEFINE_NUMINGROUP(NoCapacities);
 DEFINE_QTY(OrderCapacityQty);
 DEFINE_NUMINGROUP(NoEvents);
 DEFINE_INT(EventType);
 DEFINE_LOCALMKTDATE(EventDate);
 DEFINE_PRICE(EventPx);
 DEFINE_STRING(EventText);
 DEFINE_PERCENTAGE(PctAtRisk);
 DEFINE_NUMINGROUP(NoInstrAttrib);
 DEFINE_INT(InstrAttribType);
 DEFINE_STRING(InstrAttribValue);
 DEFINE_LOCALMKTDATE(DatedDate);
 DEFINE_LOCALMKTDATE(InterestAccrualDate);
 DEFINE_INT(CPProgram);
 DEFINE_STRING(CPRegType);
 DEFINE_STRING(UnderlyingCPProgram);
 DEFINE_STRING(UnderlyingCPRegType);
 DEFINE_QTY(UnderlyingQty);
 DEFINE_STRING(TrdMatchID);
 DEFINE_STRING(SecondaryTradeReportRefID);
 DEFINE_PRICE(UnderlyingDirtyPrice);
 DEFINE_PRICE(UnderlyingEndPrice);
 DEFINE_AMT(UnderlyingStartValue);
 DEFINE_AMT(UnderlyingCurrentValue);
 DEFINE_AMT(UnderlyingEndValue);
 DEFINE_NUMINGROUP(NoUnderlyingStips);
 DEFINE_STRING(UnderlyingStipType);
 DEFINE_STRING(UnderlyingStipValue);
 DEFINE_AMT(MaturityNetMoney);
 DEFINE_INT(MiscFeeBasis);
 DEFINE_INT(TotNoAllocs);
 DEFINE_BOOLEAN(LastFragment);
 DEFINE_STRING(CollReqID);
 DEFINE_INT(CollAsgnReason);
 DEFINE_INT(CollInquiryQualifier);
 DEFINE_NUMINGROUP(NoTrades);
 DEFINE_PERCENTAGE(MarginRatio);
 DEFINE_AMT(MarginExcess);
 DEFINE_AMT(TotalNetValue);
 DEFINE_AMT(CashOutstanding);
 DEFINE_STRING(CollAsgnID);
 DEFINE_INT(CollAsgnTransType);
 DEFINE_STRING(CollRespID);
 DEFINE_INT(CollAsgnRespType);
 DEFINE_INT(CollAsgnRejectReason);
 DEFINE_STRING(CollAsgnRefID);
 DEFINE_STRING(CollRptID);
 DEFINE_STRING(CollInquiryID);
 DEFINE_INT(CollStatus);
 DEFINE_INT(TotNumReports);
 DEFINE_BOOLEAN(LastRptRequested);
 DEFINE_STRING(AgreementDesc);
 DEFINE_STRING(AgreementID);
 DEFINE_LOCALMKTDATE(AgreementDate);
 DEFINE_LOCALMKTDATE(StartDate);
 DEFINE_LOCALMKTDATE(EndDate);
 DEFINE_CURRENCY(AgreementCurrency);
 DEFINE_INT(DeliveryType);
 DEFINE_AMT(EndAccruedInterestAmt);
 DEFINE_AMT(StartCash);
 DEFINE_AMT(EndCash);
 DEFINE_STRING(UserRequestID);
 DEFINE_INT(UserRequestType);
 DEFINE_STRING(NewPassword);
 DEFINE_INT(UserStatus);
 DEFINE_STRING(UserStatusText);
 DEFINE_INT(StatusValue);
 DEFINE_STRING(StatusText);
 DEFINE_STRING(RefCompID);
 DEFINE_STRING(RefSubID);
 DEFINE_STRING(NetworkResponseID);
 DEFINE_STRING(NetworkRequestID);
 DEFINE_STRING(LastNetworkResponseID);
 DEFINE_INT(NetworkRequestType);
 DEFINE_NUMINGROUP(NoCompIDs);
 DEFINE_INT(NetworkStatusResponseType);
 DEFINE_NUMINGROUP(NoCollInquiryQualifier);
 DEFINE_INT(TrdRptStatus);
 DEFINE_INT(AffirmStatus);
 DEFINE_CURRENCY(UnderlyingStrikeCurrency);
 DEFINE_CURRENCY(LegStrikeCurrency);
 DEFINE_STRING(TimeBracket);
 DEFINE_INT(CollAction);
 DEFINE_INT(CollInquiryStatus);
 DEFINE_INT(CollInquiryResult);
 DEFINE_CURRENCY(StrikeCurrency);
 DEFINE_NUMINGROUP(NoNested3PartyIDs);
 DEFINE_STRING(Nested3PartyID);
 DEFINE_CHAR(Nested3PartyIDSource);
 DEFINE_INT(Nested3PartyRole);
 DEFINE_NUMINGROUP(NoNested3PartySubIDs);
 DEFINE_STRING(Nested3PartySubID);
 DEFINE_INT(Nested3PartySubIDType);
 DEFINE_MONTHYEAR(LegContractSettlMonth);
 DEFINE_LOCALMKTDATE(LegInterestAccrualDate);
} // namespace FIX
} // namespace Kway

#endif //FIX_FIELDS_H
