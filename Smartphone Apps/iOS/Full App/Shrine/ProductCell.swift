/*
 Copyright 2018-present the Material Components for iOS authors. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

import UIKit
import Charts
import MaterialComponents

protocol ExpandedCellDelegate:NSObjectProtocol{
    func topButtonTouched(indexPath:IndexPath)
}

//TODO: Change from a UICollectionViewCell to an MDCCardCollectionCell
class ProductCell: MDCCardCollectionCell {
    
  @IBOutlet weak var imageView: UIImageView!
  @IBOutlet weak var nameLabel: UILabel!
  @IBOutlet weak var priceLabel: UILabel!
  @IBOutlet weak var expandButton: UIButton!
    @IBOutlet weak var unit: UILabel!
    @IBOutlet weak var statusbar: UIImageView!
    @IBOutlet weak var statuspoint: UILabel!
    @IBOutlet weak var linechart: LineChartView!
    let line = LineChartDataSet()
    var visitors: [Double] = []
    public var expanded : Bool = false
    private var expandedHeight : CGFloat = 200
    private var notExpandedHeight : CGFloat = 50
    
    override func awakeFromNib() {
    super.awakeFromNib()
        
    //TODO: Configure the cell properties
    line.label = " "
    self.backgroundColor = .white
    self.priceLabel.textColor = GMColor.imperialblue()
    self.nameLabel.textColor = GMColor.imperialblue()
    self.tintColor = .white
    self.imageView.image?.withRenderingMode(.alwaysTemplate)
    self.imageView.tintColor = .white
    //TODO: Configure the MDCCardCollectionCell specific properties
    self.cornerRadius = 4.0;
    self.setBorderWidth(3.0, for:.normal)
    self.setBorderColor(GMColor.imperialblue(), for: .normal)
        linechart.leftAxis.drawGridLinesEnabled = false
        linechart.xAxis.drawGridLinesEnabled = false
        linechart.xAxis.drawLabelsEnabled = false
    
  }
    func changelable(x : String){
        self.priceLabel.text = x
    }
//*******************************Expanding attempt *************************
    weak var delegate:ExpandedCellDelegate?
    
    public var indexPath:IndexPath!
    
    @IBAction func onclick(_ sender: UIButton) {
        print("im touched")
        if let delegate = self.delegate{
            delegate.topButtonTouched(indexPath: indexPath)
        }
    }
    func handleTap(sender: UITapGestureRecognizer) {
        
        print("called swipe")
        
    }
     func updateChart() {
        var chartEntry = [ChartDataEntry]()
        
        for i in 0..<visitors.count {
            let value = ChartDataEntry(x: Double(i), y: visitors[i])
            chartEntry.append(value)
        }
        
        line.values = chartEntry
        line.colors = [GMColor.imperialorange()]
        line.lineWidth = 4
        line.drawCircleHoleEnabled = false
        line.drawCirclesEnabled = false
        line.drawValuesEnabled = false
        line.drawHorizontalHighlightIndicatorEnabled = false
        line.drawVerticalHighlightIndicatorEnabled = false
        
        
        let data = LineChartData()
        data.addDataSet(line)
        
        linechart.data = data
        linechart.chartDescription?.text = " "

    }
   


}
